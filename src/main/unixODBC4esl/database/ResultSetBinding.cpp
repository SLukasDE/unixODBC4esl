/*
 * This file is part of unixODBC4esl.
 * Copyright (C) 2021 Sven Lukas
 *
 * UnixODBC4esl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UnixODBC4esl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with mhd4esl.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <unixODBC4esl/database/ResultSetBinding.h>
#include <unixODBC4esl/database/Driver.h>
#include <unixODBC4esl/Logger.h>

#include <esl/Stacktrace.h>

#include <stdexcept>
#include <limits>

namespace unixODBC4esl {
namespace database {

namespace {
Logger logger("unixODBC4esl::database::ResultSetBinding");
}

ResultSetBinding::ResultSetBinding(StatementHandle&& aStatementHandle, const std::vector<esl::database::Column>& resultColumns/*, const std::vector<esl::database::Column>& parameterColumns, const std::vector<esl::database::Field>& parameterFields*/)
: esl::database::ResultSet::Binding(resultColumns),
  statementHandle(std::move(aStatementHandle)),
  resultVariables(resultColumns.size())
{
	for(std::size_t i=0; i<getColumns().size(); ++i) {
		resultVariables[i].valueResultLength = 0;

		switch(getColumns()[i].getType()) {
		case esl::database::Column::Type::sqlInteger:
		case esl::database::Column::Type::sqlSmallInt:
			Driver::getDriver().bindCol(statementHandle, static_cast<SQLUSMALLINT>(i+1),
					SQL_C_SBIGINT, static_cast<SQLPOINTER>(&resultVariables[i].valueInteger), 0, &resultVariables[i].valueResultLength);
			break;

		case esl::database::Column::Type::sqlDouble:
		case esl::database::Column::Type::sqlNumeric:
		case esl::database::Column::Type::sqlDecimal:
		case esl::database::Column::Type::sqlFloat:
		case esl::database::Column::Type::sqlReal:
			Driver::getDriver().bindCol(statementHandle, static_cast<SQLUSMALLINT>(i+1),
					SQL_C_DOUBLE, static_cast<SQLPOINTER>(&resultVariables[i].valueDouble), 0, &resultVariables[i].valueResultLength);
			break;

		default:
			std::size_t valueBufferSize = getColumns()[i].getBufferSize();

			if(valueBufferSize == 0) {
				resultVariables[i].setLenght(0);
			}
			else {
				resultVariables[i].setLenght(valueBufferSize+1);
			}

			SQLLEN valueInputLength = valueBufferSize+1;
			Driver::getDriver().bindCol(statementHandle, static_cast<SQLUSMALLINT>(i+1),
					SQL_C_CHAR, static_cast<SQLPOINTER>(resultVariables[i].valueString),
					valueInputLength, &resultVariables[i].valueResultLength);
			break;
		}
	}
}

bool ResultSetBinding::fetch(std::vector<esl::database::Field>& fields) {
	if(fields.size() != getColumns().size()) {
		throw esl::addStacktrace(std::runtime_error("Called 'fetch' with wrong number of fields. Given " + std::to_string(fields.size()) + " fields, but it should be " + std::to_string(getColumns().size()) + " fields."));
	}

	if(Driver::getDriver().fetch(statementHandle) == false) {
		return false;
	}

	for(std::size_t i=0; i<getColumns().size(); ++i) {
		/* check if column value was NULL */
		if(resultVariables[i].valueResultLength == SQL_NULL_DATA) {
			fields[i] = nullptr;
			continue;
		}

		switch(getColumns()[i].getType()) {
/*
		case esl::database::Column::Type::sqlBoolean:
			if(resultVariables[i].valueString == nullptr) {
				throw esl::database::exception::RuntimeError("fetching boolean value failed, no space reserved.");
			}
			if(*resultVariables[i].valueString == '1') {
				fields[i] = true;
			}
			else if(*resultVariables[i].valueString == '0') {
				fields[i] = false;
			}
			else {
				throw esl::database::exception::RuntimeError("fetching boolean value failed, invalid value.");
			}
			break;
*/
		case esl::database::Column::Type::sqlInteger:
		case esl::database::Column::Type::sqlSmallInt:
			logger.debug << "Set integer\n";
			fields[i] = resultVariables[i].valueInteger;
			logger.debug << "Set integer done\n";
			break;

		case esl::database::Column::Type::sqlDouble:
		case esl::database::Column::Type::sqlNumeric:
		case esl::database::Column::Type::sqlDecimal:
		case esl::database::Column::Type::sqlFloat:
		case esl::database::Column::Type::sqlReal:
			logger.debug << "Set double\n";
			fields[i] = resultVariables[i].valueDouble;
			logger.debug << "Set double done\n";
			break;

		case esl::database::Column::Type::sqlVarChar:
		case esl::database::Column::Type::sqlChar:
		default:
			/* check if we have to get data manually */
			if(resultVariables[i].valueResultLength == SQL_NO_TOTAL
					|| static_cast<std::size_t>(resultVariables[i].valueResultLength) > getColumns()[i].getBufferSize()) {
				std::string stringGetData;
				BindVariable tmpBindVariable;

				if(resultVariables[i].valueResultLength == SQL_NO_TOTAL) {
					tmpBindVariable.setLenght(getColumns()[i].getDefaultBufferSize());
				}
				else {
					tmpBindVariable.setLenght(resultVariables[i].valueResultLength+1);
				}

				while(true) {
					Driver::getDriver().getData(statementHandle, static_cast<SQLUSMALLINT>(i+1),
							SQL_C_CHAR, tmpBindVariable.valueString,
							tmpBindVariable.getLength(), &tmpBindVariable.valueResultLength);

					if(tmpBindVariable.valueResultLength == SQL_NULL_DATA) {
						break;
					}
					else if(tmpBindVariable.valueResultLength == SQL_NO_TOTAL) {
						stringGetData += std::string(tmpBindVariable.valueString, tmpBindVariable.getLength()-1);
						continue;
					}

					stringGetData += std::string(tmpBindVariable.valueString, tmpBindVariable.valueResultLength);
					break;
				}

				if(tmpBindVariable.valueResultLength == SQL_NULL_DATA) {
					logger.warn << "Automatically fetching of column \"" << getColumns()[i].getName() << "\" was SQL_NO_TOTAL but getData() got SQL_NULL_DATA result.\n";
					fields[i] = nullptr;
				}
				else {
					fields[i] = std::move(stringGetData);
				}
			}
			else {
				//resultVariables[i].valueString[getColumns()[i].getBufferSize()] = 0;
				fields[i] = std::string(resultVariables[i].valueString, resultVariables[i].valueResultLength);
			}
			break;
		}
	}

	return true;
}

bool ResultSetBinding::isEditable(std::size_t columnIndex) {
	return false;
}

void ResultSetBinding::add(std::vector<esl::database::Field>& fields) {
    throw esl::addStacktrace(std::runtime_error("add not allowed for query result set."));
}

void ResultSetBinding::save(std::vector<esl::database::Field>& fields) {
    throw esl::addStacktrace(std::runtime_error("save not allowed for query result set."));
}

} /* namespace database */
} /* namespace unixODBC4esl */
