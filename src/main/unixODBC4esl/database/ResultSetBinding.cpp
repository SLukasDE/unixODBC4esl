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
  bindResult(resultColumns.size())
{
	logger.trace << "Bind result variables\":\n";
	logger.trace << "-----------------------------------------------\n";
	for(std::size_t i=0; i<getColumns().size(); ++i) {
		bindResult[i].reset(new BindResult(statementHandle, getColumns()[i], i));
	}
	logger.trace << "-----------------------------------------------\n\n";
}

bool ResultSetBinding::fetch(std::vector<esl::database::Field>& fields) {
	if(fields.size() != getColumns().size()) {
		throw esl::addStacktrace(std::runtime_error("Called 'fetch' with wrong number of fields. Given " + std::to_string(fields.size()) + " fields, but it should be " + std::to_string(getColumns().size()) + " fields."));
	}

	if(Driver::getDriver().fetch(statementHandle) == false) {
		return false;
	}

	logger.trace << "Fetch, set fields (" << getColumns().size() << "):\n";
	logger.trace << "-----------------------------------------------\n";
	for(std::size_t i=0; i<getColumns().size(); ++i) {
		if(logger.trace) {
			logger.trace << "Column " << i << ":\n";
			logger.trace << "    Name: \"" << getColumns()[i].getName() << "\"\n";
			switch(getColumns()[i].getType()) {
			case esl::database::Column::Type::sqlBoolean:
				logger.trace << "    Type: sqlBoolean\n";
				break;
			case esl::database::Column::Type::sqlInteger:
				logger.trace << "    Type: sqlInteger\n";
				break;
			case esl::database::Column::Type::sqlSmallInt:
				logger.trace << "    Type: sqlSmallInt\n";
				break;
			case esl::database::Column::Type::sqlDouble:
				logger.trace << "    Type: sqlDouble\n";
				break;
			case esl::database::Column::Type::sqlNumeric:
				logger.trace << "    Type: sqlNumeric\n";
				break;
			case esl::database::Column::Type::sqlDecimal:
				logger.trace << "    Type: sqlDecimal\n";
				break;
			case esl::database::Column::Type::sqlFloat:
				logger.trace << "    Type: sqlFloat\n";
				break;
			case esl::database::Column::Type::sqlReal:
				logger.trace << "    Type: sqlReal\n";
				break;
			case esl::database::Column::Type::sqlVarChar:
				logger.trace << "    Type: sqlVarChar\n";
				break;
			case esl::database::Column::Type::sqlChar:
				logger.trace << "    Type: sqlChar\n";
				break;
			case esl::database::Column::Type::sqlDateTime:
				logger.trace << "    Type: sqlDateTime\n";
				break;
			case esl::database::Column::Type::sqlDate:
				logger.trace << "    Type: sqlDate\n";
				break;
			case esl::database::Column::Type::sqlTime:
				logger.trace << "    Type: sqlTime\n";
				break;
			case esl::database::Column::Type::sqlTimestamp:
				logger.trace << "    Type: sqlTimestamp\n";
				break;
			case esl::database::Column::Type::sqlWChar:
				logger.trace << "    Type: sqlWChar\n";
				break;
			case esl::database::Column::Type::sqlWVarChar:
				logger.trace << "    Type: sqlWVarChar\n";
				break;
			case esl::database::Column::Type::sqlWLongVarChar:
				logger.trace << "    Type: sqlWLongVarChar\n";
				break;
			default:
				logger.trace << "    Type: sqlUnknown\n";
				break;
			}
		}


		bindResult[i]->setField(fields[i]);
	}
	logger.trace << "-----------------------------------------------\n\n";

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
