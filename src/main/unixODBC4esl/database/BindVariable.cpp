/*
 * This file is part of unixODBC4esl.
 * Copyright (C) 2020-2022 Sven Lukas
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

#include <unixODBC4esl/database/BindVariable.h>
#include <unixODBC4esl/database/Driver.h>
#include <unixODBC4esl/Logger.h>

#include <string.h> // memcpy

namespace unixODBC4esl {
namespace database {

namespace {
Logger logger("unixODBC4esl::database::BindVariable");
}

BindVariable::BindVariable(const StatementHandle& aStatementHandle, const esl::database::Column& aColumn, std::size_t aIndex)
: statementHandle(aStatementHandle),
  column(aColumn),
  index(aIndex)
{
	valueString = nullptr;
}

BindVariable::~BindVariable() {
	switch(column.getType()) {
	case esl::database::Column::Type::sqlInteger:
	case esl::database::Column::Type::sqlSmallInt:
	case esl::database::Column::Type::sqlDouble:
	case esl::database::Column::Type::sqlNumeric:
	case esl::database::Column::Type::sqlDecimal:
	case esl::database::Column::Type::sqlFloat:
	case esl::database::Column::Type::sqlReal:
		break;
	default:
		if(valueString) {
			delete[] valueString;
		}
		break;
	}
}

void BindVariable::getField(const esl::database::Field& field) {
	//switch(parameterValues[i].getColumnType()) {
	switch(column.getType()) {
	case esl::database::Column::Type::sqlInteger:
	case esl::database::Column::Type::sqlSmallInt:
		if(field.isNull()) {
			resultLength = SQL_NULL_DATA;
		}
		else {
			resultLength = 0;
			valueInteger = field.asInteger();
		}

		Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(index+1), SQL_PARAM_INPUT,
				SQL_C_SBIGINT, Driver::columnType2SqlType(column.getType()),
				column,
				static_cast<SQLPOINTER>(&valueInteger),
				0,
				&resultLength);
		break;

	case esl::database::Column::Type::sqlDouble:
	case esl::database::Column::Type::sqlNumeric:
	case esl::database::Column::Type::sqlDecimal:
	case esl::database::Column::Type::sqlFloat:
	case esl::database::Column::Type::sqlReal:
		if(field.isNull()) {
			resultLength = SQL_NULL_DATA;
		}
		else {
			resultLength = 0;
			valueDouble = field.asDouble();
		}

		Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(index+1), SQL_PARAM_INPUT,
				SQL_C_DOUBLE, Driver::columnType2SqlType(column.getType()),
				column,
				static_cast<SQLPOINTER>(&valueDouble),
				0,
				&resultLength);
		break;

	default:
		if(valueString) {
			delete[] valueString;
			valueString = nullptr;
		}

		SQLLEN bufferLength = 0;
		if(field.isNull()) {
			resultLength = SQL_NULL_DATA;
		}
		else {
			std::string str = field.asString();
			bufferLength = str.size();
			resultLength = str.size();

			valueString = new char[resultLength+1];
			logger.trace << "new char[" << (resultLength + 1) << "]\n";
			memcpy(valueString, str.data(), resultLength);
			valueString[resultLength] = 0;
		}

		/* ..., SQL_C_CHAR, SQL_CHAR,
		 * parameterColumns[i]  ( with .getCharacterLength() = 255 / .getDecimalDigits() = 0)  ,
		 * &parameterVariables[i].valueString,
		 * 255,
		 * &parameterVariables[i].valueResultLength = str.size();
		 */
		Driver::getDriver().bindParameter(statementHandle, static_cast<SQLUSMALLINT>(index+1), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR,
				column,
				static_cast<SQLPOINTER>(valueString),
				bufferLength+1,
				&resultLength);
		break;
	}

	if(logger.trace) {
		logger.trace << "Parameter " << index << ":\n";
		//SQLSMALLINT sqlType = Driver::columnType2SqlType(column.getType());
		logger.trace << "  Column:\n";
		logger.trace << "    getBufferSize()      = " << column.getBufferSize() << "\n";
		logger.trace << "    getCharacterLength() = " << column.getCharacterLength() << "\n";
		logger.trace << "    getDisplayLength()   = " << column.getDisplayLength() << "\n";
		logger.trace << "    getDecimalDigits()   = " << column.getDecimalDigits() << "\n";
		//logger.trace << "    getType()            = " << column.getType() << "\n";
		logger.trace << "    sqlType              = " << Driver::columnType2SqlType(column.getType()) << "\n";
		logger.trace << "  Parameter:\n";
		logger.trace << "    getTypeName()        = " << field.getTypeName() << "\n";
		//logger.trace << "    getColumnType()      = " << field.getColumnType() << "\n";
		logger.trace << "    sqlType              = " << Driver::columnType2SqlType(field.getColumnType()) << "\n";
		logger.trace << "\n";

		switch(column.getType()) {
		case esl::database::Column::Type::sqlBoolean:
			logger.trace << "    Column-Type: sqlBoolean\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlInteger:
			logger.trace << "    Column-Type: sqlInteger\n";
			logger.trace << "    -> USE field.asInteger\n";
			break;
		case esl::database::Column::Type::sqlSmallInt:
			logger.trace << "    Column-Type: sqlSmallInt\n";
			logger.trace << "    -> USE field.asInteger\n";
			break;
		case esl::database::Column::Type::sqlDouble:
			logger.trace << "    Column-Type: sqlDouble\n";
			logger.trace << "    -> USE field.asDouble\n";
			break;
		case esl::database::Column::Type::sqlNumeric:
			logger.trace << "    Column-Type: sqlNumeric\n";
			logger.trace << "    -> USE field.asDouble\n";
			break;
		case esl::database::Column::Type::sqlDecimal:
			logger.trace << "    Column-Type: sqlDecimal\n";
			logger.trace << "    -> USE field.asDouble\n";
			break;
		case esl::database::Column::Type::sqlFloat:
			logger.trace << "    Column-Type: sqlFloat\n";
			logger.trace << "    -> USE field.asDouble\n";
			break;
		case esl::database::Column::Type::sqlReal:
			logger.trace << "    Column-Type: sqlReal\n";
			logger.trace << "    -> USE field.asDouble\n";
			break;
		case esl::database::Column::Type::sqlVarChar:
			logger.trace << "    Column-Type: sqlVarChar\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlChar:
			logger.trace << "    Column-Type: sqlChar\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlDateTime:
			logger.trace << "    Column-Type: sqlDateTime\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlDate:
			logger.trace << "    Column-Type: sqlDate\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlTime:
			logger.trace << "    Column-Type: sqlTime\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlTimestamp:
			logger.trace << "    Column-Type: sqlTimestamp\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlWChar:
			logger.trace << "    Column-Type: sqlWChar\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlWVarChar:
			logger.trace << "    Column-Type: sqlWVarChar\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		case esl::database::Column::Type::sqlWLongVarChar:
			logger.trace << "    Column-Type: sqlWLongVarChar\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		default:
			logger.trace << "    Column-Type: sqlUnknown\n";
			logger.trace << "    -> USE field.asString\n";
			if(!field.isNull()) {
				logger.trace << "       setLenght(" << (field.asString().size() + 1) << ")\n";
			}
			break;
		}
	}
}

} /* namespace database */
} /* namespace unixODBC4esl */
