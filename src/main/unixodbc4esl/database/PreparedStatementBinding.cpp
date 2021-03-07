/*
 * This file is part of unixodbc4esl.
 * Copyright (C) 2021 Sven Lukas
 *
 * Unixodbc4esl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Unixodbc4esl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with mhd4esl.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <unixodbc4esl/database/PreparedStatementBinding.h>
#include <unixodbc4esl/database/Driver.h>
#include <unixodbc4esl/database/ResultSetBinding.h>
#include <unixodbc4esl/Logger.h>

#include <esl/Stacktrace.h>

#include <sqlext.h>

#include <stdexcept>
#include <memory>

namespace unixodbc4esl {
namespace database {

namespace {
Logger logger("unixodbc4esl::database::PreparedStatementBinding");
}

PreparedStatementBinding::PreparedStatementBinding(const Connection& aConnection, const std::string& aSql, std::size_t defaultBufferSize, std::size_t maximumBufferSize)
: connection(aConnection),
  sql(aSql),
  statementHandle(Driver::getDriver().prepare(connection, sql))
{
	// Get number of columns from prepared statement
	SQLSMALLINT resultColumnsCount = Driver::getDriver().numResultCols(statementHandle);

	for(SQLSMALLINT i=0; i<resultColumnsCount; ++i) {
		std::string resultColumnName;
		esl::database::Column::Type resultColumnType;
		bool resultValueNullable;
		std::size_t resultValueCharacterLength;
		std::size_t resultValueDecimalDigits;
		std::size_t resultValueDisplayLength;

		Driver::getDriver().describeCol(statementHandle, i+1, resultColumnName, resultColumnType, resultValueCharacterLength, resultValueDecimalDigits, resultValueNullable);
		Driver::getDriver().colAttributeDisplaySize(statementHandle, i+1, resultValueDisplayLength);

		resultColumns.emplace_back(std::move(resultColumnName), resultColumnType, resultValueNullable, defaultBufferSize, maximumBufferSize, resultValueCharacterLength, resultValueDecimalDigits, resultValueDisplayLength);
    }

	SQLSMALLINT parameterColumnsCount = Driver::getDriver().numParams(statementHandle);

	for(SQLSMALLINT i=0; i<parameterColumnsCount; ++i) {
		esl::database::Column::Type parameterColumnType;
		std::size_t parameterValueCharacterLength;
		std::size_t parameterValueDecimalDigits;
		bool parameterValueNullable;

		Driver::getDriver().describeParam(statementHandle, i+1, parameterColumnType, parameterValueCharacterLength, parameterValueDecimalDigits, parameterValueNullable);

		parameterColumns.emplace_back("", parameterColumnType, parameterValueNullable, defaultBufferSize, maximumBufferSize, parameterValueDecimalDigits, parameterValueCharacterLength, parameterValueCharacterLength);
    }
}

const std::vector<esl::database::Column>& PreparedStatementBinding::getParameterColumns() const {
	return parameterColumns;
}

const std::vector<esl::database::Column>& PreparedStatementBinding::getResultColumns() const {
	return resultColumns;
}

esl::database::ResultSet PreparedStatementBinding::execute(const std::vector<esl::database::Field>& parameterValues) {
	if(!statementHandle) {
		logger.trace << "RE-Create statement handle\n";
		statementHandle = StatementHandle(Driver::getDriver().prepare(connection, sql));
	}

	if(parameterColumns.size() != parameterValues.size()) {
	    throw esl::addStacktrace(std::runtime_error("Wrong number of arguments. Given " + std::to_string(parameterValues.size()) + " parameters but required " + std::to_string(parameterColumns.size()) + " parameters."));
	}

	std::vector<BindVariable> parameterVariables(parameterValues.size());

	for(std::size_t i=0; i<parameterValues.size(); ++i) {
		SQLSMALLINT sqlType = Driver::columnType2SqlType(parameterColumns[i].getType());
		SQLLEN bufferLength = 0;

		logger.debug << "Bind parameter[" << i << "]\n";
		logger.debug << "  getTypeName()        = " << parameterColumns[i].getTypeName() << "\n";
		logger.debug << "  sqlType              = " << sqlType << "\n";
		logger.debug << "  field.sqlType        = " << Driver::columnType2SqlType(parameterValues[i].getColumnType()) << "\n";
		logger.debug << "  getBufferSize()      = " << parameterColumns[i].getBufferSize() << "\n";
		logger.debug << "  getCharacterLength() = " << parameterColumns[i].getCharacterLength() << "\n";
		logger.debug << "  getDisplayLength()   = " << parameterColumns[i].getDisplayLength() << "\n";
		logger.debug << "  getDecimalDigits()   = " << parameterColumns[i].getDecimalDigits() << "\n";

		switch(parameterColumns[i].getType()) {
		case esl::database::Column::Type::sqlInteger:
		case esl::database::Column::Type::sqlSmallInt:
			logger.debug << "  USE field.asInteger\n";
			if(parameterValues[i].isNull()) {
				parameterVariables[i].valueResultLength = SQL_NULL_DATA;
			}
			else {
				parameterVariables[i].valueResultLength = 0;
				parameterVariables[i].valueInteger = parameterValues[i].asInteger();
			}

			Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(i+1), SQL_PARAM_INPUT,
					SQL_C_SBIGINT, sqlType,
					parameterColumns[i],
					static_cast<SQLPOINTER>(&parameterVariables[i].valueInteger),
					bufferLength,
					&parameterVariables[i].valueResultLength);
			break;

		case esl::database::Column::Type::sqlDouble:
		case esl::database::Column::Type::sqlNumeric:
		case esl::database::Column::Type::sqlDecimal:
		case esl::database::Column::Type::sqlFloat:
		case esl::database::Column::Type::sqlReal:
			logger.debug << "  USE field.asDouble\n";
			if(parameterValues[i].isNull()) {
				parameterVariables[i].valueResultLength = SQL_NULL_DATA;
			}
			else {
				parameterVariables[i].valueResultLength = 0;
				parameterVariables[i].valueDouble = parameterValues[i].asDouble();
			}

			Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(i+1), SQL_PARAM_INPUT,
					SQL_C_DOUBLE, sqlType,
					parameterColumns[i],
					static_cast<SQLPOINTER>(&parameterVariables[i].valueDouble),
					bufferLength,
					&parameterVariables[i].valueResultLength);
			break;

		default:
			logger.debug << "  USE field.asString\n";
			if(parameterValues[i].isNull()) {
				parameterVariables[i].valueResultLength = SQL_NULL_DATA;
			}
			else {
				std::string str = parameterValues[i].asString();
				bufferLength = str.size();
				parameterVariables[i].valueResultLength = bufferLength+1;

				logger.trace << "parameterVariables[" << i << "].setLenght(" << (str.size() + 1) << ")\n";
				parameterVariables[i].setLenght(str.size() + 1);
				memcpy(parameterVariables[i].valueString, str.data(), str.size());
				parameterVariables[i].valueString[str.size()] = 0;
			}

			Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(i+1), SQL_PARAM_INPUT,
					SQL_C_CHAR, SQL_CHAR,
					parameterColumns[i],
					static_cast<SQLPOINTER>(&parameterVariables[i].valueString),
					bufferLength+1,
					&parameterVariables[i].valueResultLength);
			break;
		}
	}

	/* ResultSetBinding makes the "execute" */
	Driver::getDriver().execute(statementHandle);

	esl::database::ResultSet resultSet;

	/* make a fetch, if SQL statement has result set (e.g. no INSERT, UPDATE, DELETE) */
	if(!resultColumns.empty()) {
		std::unique_ptr<esl::database::ResultSet::Binding> resultSetBinding(new ResultSetBinding(std::move(statementHandle), resultColumns));

		/* this makes a fetch */
		resultSet = esl::database::ResultSet(std::unique_ptr<esl::database::ResultSet::Binding>(std::move(resultSetBinding)));
	}

	return resultSet;
}

void PreparedStatementBinding::executeBulk(const std::vector<std::vector<esl::database::Field>>& fieldArrays) {
}

void* PreparedStatementBinding::getNativeHandle() const {
	if(statementHandle) {
		return statementHandle.getHandle();
	}
	return nullptr;
}

} /* namespace database */
} /* namespace unixodbc4esl */
