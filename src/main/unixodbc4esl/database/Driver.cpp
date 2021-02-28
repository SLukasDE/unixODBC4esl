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

#include <unixodbc4esl/database/Driver.h>
#include <unixodbc4esl/database/Diagnostics.h>
#include <unixodbc4esl/Logger.h>

#include <esl/database/exception/SqlError.h>
#include <esl/logging/Location.h>
#include <esl/Stacktrace.h>

#include <algorithm>
#include <stdexcept>
#include <memory>

namespace unixodbc4esl {
namespace database {

namespace {
Logger logger("unixodbc4esl::database::Driver");

void checkAndThrow(SQLRETURN rc, SQLSMALLINT type, SQLHANDLE handle, const char* operation) {
    switch(rc) {
    case SQL_INVALID_HANDLE:
    	if(operation) {
            throw esl::addStacktrace(std::runtime_error(std::string("invalid handle for calling ") + operation));
    	}
    	else {
            throw esl::addStacktrace(std::runtime_error("invalid handle"));
    	}
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
    	break;
    case SQL_ERROR:
    default:
    	if(operation) {
            throw esl::addStacktrace(esl::database::exception::SqlError(Diagnostics(type, handle), rc, operation));
    	}
    	else {
            throw esl::addStacktrace(esl::database::exception::SqlError(Diagnostics(type, handle), rc));
    	}
    }
}
}

const Driver& Driver::getDriver() {
	static std::unique_ptr<Driver> driver;

	if(!driver) {
		driver.reset(new Driver);
	}

	return *driver;
}

esl::database::Column::Type Driver::sqlType2ColumnType(SQLSMALLINT sqlType) {
	switch(sqlType) {
//	case SQL_BOOLEAN:
//		return esl::database::Column::Type::sqlBoolean;

	case SQL_INTEGER:
		return esl::database::Column::Type::sqlInteger;
	case SQL_SMALLINT:
		return esl::database::Column::Type::sqlSmallInt;

	case SQL_DOUBLE:
		return esl::database::Column::Type::sqlDouble;
	case SQL_NUMERIC:
		return esl::database::Column::Type::sqlNumeric;
	case SQL_DECIMAL:
		return esl::database::Column::Type::sqlDecimal;
	case SQL_FLOAT:
		return esl::database::Column::Type::sqlFloat;
	case SQL_REAL:
		return esl::database::Column::Type::sqlReal;

	case SQL_CHAR:
		return esl::database::Column::Type::sqlChar;
	case SQL_VARCHAR:
		return esl::database::Column::Type::sqlVarChar;

	case SQL_DATETIME:
		return esl::database::Column::Type::sqlDateTime;
	case SQL_TYPE_DATE:
		return esl::database::Column::Type::sqlDate;
	case SQL_TYPE_TIME:
		return esl::database::Column::Type::sqlTime;
	case SQL_TYPE_TIMESTAMP:
		return esl::database::Column::Type::sqlTimestamp;
//	case SQL_TYPE_TIMESTAMP_WITH_TIMEZONE:
//		return esl::database::Column::Type::sqlTimestampWithTimezone;

//	case SQL_ROW:
//		return esl::database::Column::Type::sqlRow;
	case SQL_WCHAR:
		return esl::database::Column::Type::sqlWChar;
	case SQL_WVARCHAR:
		return esl::database::Column::Type::sqlWVarChar;
	case SQL_WLONGVARCHAR:
		return esl::database::Column::Type::sqlWLongVarChar;
//	case SQL_DECFLOAT:
//		return esl::database::Column::Type::sqlDecFloat;
	default:
		break;
	}

	return esl::database::Column::Type::sqlUnknown;
}

SQLSMALLINT Driver::columnType2SqlType(esl::database::Column::Type columnType) {
	switch(columnType) {
//	case esl::database::Column::Type::sqlBoolean:
//		return SQL_BOOLEAN;

	case esl::database::Column::Type::sqlInteger:
		return SQL_INTEGER;
	case esl::database::Column::Type::sqlSmallInt:
		return SQL_SMALLINT;

	case esl::database::Column::Type::sqlDouble:
		return SQL_DOUBLE;
	case esl::database::Column::Type::sqlNumeric:
		return SQL_NUMERIC;
	case esl::database::Column::Type::sqlDecimal:
		return SQL_DECIMAL;
	case esl::database::Column::Type::sqlFloat:
		return SQL_FLOAT;
	case esl::database::Column::Type::sqlReal:
		return SQL_REAL;

	case esl::database::Column::Type::sqlChar:
		return SQL_CHAR;
	case esl::database::Column::Type::sqlVarChar:
		return SQL_VARCHAR;

	case esl::database::Column::Type::sqlDateTime:
		return SQL_DATETIME;
	case esl::database::Column::Type::sqlDate:
		return SQL_TYPE_DATE;
	case esl::database::Column::Type::sqlTime:
		return SQL_TYPE_TIME;
	case esl::database::Column::Type::sqlTimestamp:
		return SQL_TYPE_TIMESTAMP;
//	case esl::database::Column::Type::sqlTimestampWithTimezone:
//		return SQL_TYPE_TIMESTAMP_WITH_TIMEZONE;

//	case esl::database::Column::Type::sqlRow:
//		return SQL_ROW;
	case esl::database::Column::Type::sqlWChar:
		return SQL_WCHAR;
	case esl::database::Column::Type::sqlWVarChar:
		return SQL_WVARCHAR;
	case esl::database::Column::Type::sqlWLongVarChar:
		return SQL_WLONGVARCHAR;
//	case esl::database::Column::Type::sqlDecFloat:
//		return SQL_DECFLOAT;
	default:
		break;
	}

	return SQL_UNKNOWN_TYPE;
}

SQLHANDLE Driver::allocHandleEnvironment() const {
	SQLHANDLE newHandle;
	SQLRETURN rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &newHandle);

	checkAndThrow(rc, SQL_HANDLE_ENV, SQL_NULL_HENV, "SQLAllocHandle for environment");

	return newHandle;
}

SQLHANDLE Driver::allocHandleConnection(const ConnectionFactory& connectionFactory) const {
	SQLHANDLE newHandle;
	SQLRETURN rc = SQLAllocHandle(SQL_HANDLE_DBC, connectionFactory.getHandle(), &newHandle);

	checkAndThrow(rc, SQL_HANDLE_ENV, connectionFactory.getHandle(), "SQLAllocHandle for connection");

	return newHandle;
}

void Driver::freeHandle(const ConnectionFactory& connectionFactory) const {
	SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_ENV, connectionFactory.getHandle());

	checkAndThrow(rc, SQL_HANDLE_ENV, connectionFactory.getHandle(), "SQLFreeHandle for environment handle");
}

void Driver::freeHandle(const Connection& connection) const {
	SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_DBC, connection.getHandle());

	checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLFreeHandle for connection handle");
}

void Driver::freeHandle(const StatementHandle& statementHandle) const {
	SQLRETURN rc = SQLFreeHandle(SQL_HANDLE_DBC, statementHandle.getHandle());

	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLFreeHandle for statement handle");
}

#if 0
void Driver::freeHandle(SQLSMALLINT type, SQLHANDLE handle) const {
	SQLRETURN rc = SQLFreeHandle(type, handle);

	switch(type) {
	/*
	case SQL_HANDLE_ENV:
		checkAndThrow(rc, handle, "SQLFreeHandle for environment handle");
		handle.handle = SQL_NULL_HENV;
		break;
	case SQL_HANDLE_DBC:
		checkAndThrow(rc, handle, "SQLFreeHandle for connection handle");
		handle.handle = SQL_NULL_HDBC;
		break;
	case SQL_HANDLE_STMT:
		checkAndThrow(rc, handle, "SQLFreeHandle for statement handle");
		handle.handle = SQL_NULL_HSTMT;
		break;
	*/
	case SQL_HANDLE_DESC:
		checkAndThrow(rc, type, handle, "SQLFreeHandle for descriptor handle");
		break;
	default:
		checkAndThrow(rc, type, handle, "SQLFreeHandle");
		break;
	}
}
#endif

void Driver::setEnvAttr(const ConnectionFactory& connectionFactory, SQLINTEGER attribute, SQLPOINTER value, SQLINTEGER stringLength) const {
	SQLRETURN rc = SQLSetEnvAttr(connectionFactory.getHandle(), attribute, value, stringLength);

	checkAndThrow(rc, SQL_HANDLE_ENV, connectionFactory.getHandle(), "SQLSetEnvAttr");
}

void Driver::setConnectAttr(const Connection& connection, SQLINTEGER attribute, SQLPOINTER value, SQLINTEGER stringLength) const {
	SQLRETURN rc = SQLSetConnectAttr(connection.getHandle(), attribute, value, stringLength);
	checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLSetConnectAttr");
}

void Driver::driverConnect(const Connection& connection, const std::string connectionString) const {
	ESL__LOGGER_TRACE_THIS("connect\n");

    SQLCHAR* szConnStrIn = reinterpret_cast<SQLCHAR*>(const_cast<char*>(connectionString.c_str()));
    SQLSMALLINT cbConnStrIn = connectionString.size();

	SQLRETURN rc = SQLDriverConnect(connection.getHandle(), NULL, szConnStrIn, cbConnStrIn, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
	checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLDriverConnect");

	ESL__LOGGER_TRACE_THIS("connected\n");
}

void Driver::endTran(const Connection& connection, SQLSMALLINT type) const {
    SQLRETURN rc;

    rc = SQLEndTran(SQL_HANDLE_DBC, connection.getHandle(), type);
    switch(type) {
    case SQL_COMMIT:
        checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLEndTran with SQL_COMMIT");
        break;
    case SQL_ROLLBACK:
        checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLEndTran with SQL_ROLLBACK");
        break;
    default:
        checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLEndTran");
    }
}

void Driver::disconnect(const Connection& connection) const {
    SQLRETURN rc;

	ESL__LOGGER_TRACE_THIS("disconnect\n");
    rc = SQLDisconnect(connection.getHandle());
    checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLDisconnect");

	ESL__LOGGER_TRACE_THIS("free connection handle\n");
    freeHandle(connection);

	ESL__LOGGER_TRACE_THIS("disconnected\n");
}

bool Driver::getDiagRec(esl::database::Diagnostic& diagnostic, SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT index) const {
	SQLRETURN rc;
	SQLINTEGER sqlcode;
	SQLSMALLINT length = SQL_MAX_MESSAGE_LENGTH;

	SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
	SQLCHAR message[SQL_MAX_MESSAGE_LENGTH + 1];

    rc = SQLGetDiagRec(type,
            handle,
			index,
            sqlstate,
            &sqlcode,
            message,
            SQL_MAX_MESSAGE_LENGTH,
            &length);

    if(rc != SQL_SUCCESS) {
    	return false;
    }

	sqlstate[SQL_SQLSTATE_SIZE] = 0;
    const char* bufferFrom = reinterpret_cast<char*>(sqlstate);
    const char* bufferTo = std::find(bufferFrom, bufferFrom + SQL_SQLSTATE_SIZE, 0); // find the first NUL
    diagnostic.state = std::string(bufferFrom, bufferTo);

    diagnostic.message = std::string(reinterpret_cast<char*>(message), length);

    diagnostic.code = sqlcode;

    return true;
}

StatementHandle Driver::prepare(const Connection& connection, const std::string& sql) const {
	SQLHANDLE newHandle;
	SQLRETURN rc = SQLAllocHandle(SQL_HANDLE_STMT, connection.getHandle(), &newHandle);
	checkAndThrow(rc, SQL_HANDLE_DBC, connection.getHandle(), "SQLAllocHandle for statement");

	StatementHandle statementHandle(newHandle);

	rc = SQLPrepare(statementHandle.getHandle(), reinterpret_cast<SQLCHAR*>(const_cast<char*>(sql.c_str())), SQL_NTS);
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLPrepare");

	return statementHandle;
}

SQLSMALLINT Driver::numResultCols(const StatementHandle& statementHandle) const {
	SQLSMALLINT resultColumnsCount;

	SQLRETURN rc = SQLNumResultCols(statementHandle.getHandle(), &resultColumnsCount);
    checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLNumResultCols");

	return resultColumnsCount;
}

SQLSMALLINT Driver::numParams(const StatementHandle& statementHandle) const {
	SQLSMALLINT parameterColumnsCount;

	SQLRETURN rc = SQLNumParams(statementHandle.getHandle(), &parameterColumnsCount);
    checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLNumParams");

	return parameterColumnsCount;
}

void Driver::describeCol(const StatementHandle& statementHandle, SQLSMALLINT index, std::string& resultColumnName, esl::database::Column::Type& resultColumnType, std::size_t& resultCharacterLength, std::size_t& resultDecimalDigits, bool& resultNullable) const {
	SQLCHAR     sqlResultColumnName[201];    // column name
	SQLSMALLINT sqlResultColumnNameLength;   // real length of column name
	SQLSMALLINT sqlResultColumnType;         // column type
	SQLULEN     sqlResultValueCharacterLength;        // column lengths
	SQLSMALLINT sqlResultValueDecimalDigits; // no of digits if column is numeric
	SQLSMALLINT sqlResultValueNullable;      // whether column can have NULL value

	SQLRETURN rc = SQLDescribeCol(statementHandle.getHandle(), index,
			sqlResultColumnName, sizeof(sqlResultColumnName), &sqlResultColumnNameLength, &sqlResultColumnType,
			&sqlResultValueCharacterLength, &sqlResultValueDecimalDigits, &sqlResultValueNullable);
	sqlResultColumnName[200] = 0;
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLDescribeCol()");

	resultColumnName = reinterpret_cast<char*>(&sqlResultColumnName[0]);
	resultColumnType = sqlType2ColumnType(sqlResultColumnType);
	if(resultColumnType == esl::database::Column::Type::sqlUnknown) {
		logger.warn << "describeCol called for column \"" << resultColumnName << "\" (index " << index << ") and got unknown column type " << sqlResultColumnType << ".\n";
	}
	resultCharacterLength = sqlResultValueCharacterLength < 0 ? 0 : static_cast<std::size_t>(sqlResultValueCharacterLength);
	resultDecimalDigits = sqlResultValueDecimalDigits < 0 ? 0 : static_cast<std::size_t>(sqlResultValueDecimalDigits);
	resultNullable = (sqlResultValueNullable != 0);
}

void Driver::colAttributeDisplaySize(const StatementHandle& statementHandle, SQLSMALLINT index, std::size_t& resultDisplayLength) const {
	SQLLEN      sqlResultValueDisplayLength;

	// get Maximum number of characters required to display data from the column.
	SQLRETURN rc = SQLColAttribute(statementHandle.getHandle(), index,
			SQL_COLUMN_DISPLAY_SIZE, NULL, 0, NULL, &sqlResultValueDisplayLength);
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLColAttribute()");

	resultDisplayLength = sqlResultValueDisplayLength < 0 ? 0 : static_cast<std::size_t>(sqlResultValueDisplayLength);
}

void Driver::describeParam(const StatementHandle& statementHandle, SQLSMALLINT index, esl::database::Column::Type& resultColumnType, std::size_t& resultCharacterLength, std::size_t& resultDecimalDigits, bool& resultNullable) const {
	SQLSMALLINT sqlParameterColumnType;           // column type
	SQLULEN     sqlParameterValueCharacterLength; // column lengths
	SQLSMALLINT sqlParameterValueDecimalDigits;   // no of digits if column is numeric
	SQLSMALLINT sqlParameterValueNullable;        // whether column can have NULL value

	SQLRETURN rc = SQLDescribeParam(statementHandle.getHandle(), index, &sqlParameterColumnType,
			&sqlParameterValueCharacterLength, &sqlParameterValueDecimalDigits, &sqlParameterValueNullable);
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLDescribeParam()");

	resultColumnType = sqlType2ColumnType(sqlParameterColumnType);
	resultCharacterLength = sqlParameterValueCharacterLength < 0 ? 0 : static_cast<std::size_t>(sqlParameterValueCharacterLength);
	resultDecimalDigits = sqlParameterValueDecimalDigits < 0 ? 0 : static_cast<std::size_t>(sqlParameterValueDecimalDigits);
	resultNullable = (sqlParameterValueNullable != 0);
}

void Driver::bindParameter(const StatementHandle& statementHandle, SQLSMALLINT index,
    SQLSMALLINT        paramType,
    SQLSMALLINT        cType,
    SQLSMALLINT        sqlType,
	const esl::database::Column& column,
    SQLPOINTER         rgbValue,
    SQLLEN             cbValueMax,
    SQLLEN             *pcbValue) const {
	SQLRETURN rc = SQLBindParameter(statementHandle.getHandle(), index, paramType, cType, sqlType,
			static_cast<SQLULEN>(column.getCharacterLength()), static_cast<SQLSMALLINT>(column.getDecimalDigits()),
			rgbValue, cbValueMax, pcbValue);

	switch(cType) {
	case SQL_C_SBIGINT:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindParameter() with SQL_C_SBIGINT");
		break;
	case SQL_C_DOUBLE:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindParameter() with SQL_C_DOUBLE");
		break;
	case SQL_C_CHAR:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindParameter() with SQL_C_CHAR");
		break;
	default:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindParameter()");
		break;
	}
}

void Driver::bindCol(const StatementHandle& statementHandle, SQLSMALLINT index,
		SQLSMALLINT       dataType,
		SQLPOINTER        rgbValue,
		SQLLEN            cbValueMax,
		SQLLEN            *pcbValue) const {

	SQLRETURN rc = SQLBindCol(statementHandle.getHandle(), index, dataType, rgbValue, cbValueMax, pcbValue);

	switch(dataType) {
	case SQL_C_SBIGINT:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindCol() with SQL_C_SBIGINT");
		break;
	case SQL_C_DOUBLE:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindCol() with SQL_C_DOUBLE");
		break;
	case SQL_C_CHAR:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindCol() with SQL_C_CHAR");
		break;
	default:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLBindCol()");
		break;
	}
}

void Driver::getData(const StatementHandle& statementHandle, SQLSMALLINT index,
		SQLSMALLINT dataType,
		void* dataValue,
		std::size_t dataBufferLength,
		SQLLEN* dataButterLengthOrIndicator) const {
	SQLRETURN rc = SQLGetData(statementHandle.getHandle(), index, dataType, static_cast<SQLPOINTER>(dataValue), static_cast<SQLLEN>(dataBufferLength), dataButterLengthOrIndicator);

	switch(dataType) {
	case SQL_C_SBIGINT:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLGetData() with SQL_C_SBIGINT");
		break;
	case SQL_C_DOUBLE:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLGetData() with SQL_C_DOUBLE");
		break;
	case SQL_C_CHAR:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLGetData() with SQL_C_CHAR");
		break;
	default:
		checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLGetData()");
		break;
	}
}

void Driver::execute(const StatementHandle& statementHandle) const {
	SQLRETURN rc = SQLExecute(statementHandle.getHandle());
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLExecute()");
}

bool Driver::fetch(const StatementHandle& statementHandle) const {
	SQLRETURN rc = SQLFetch(statementHandle.getHandle());
	if(rc == SQL_NO_DATA) {
		return false;
	}
	checkAndThrow(rc, SQL_HANDLE_STMT, statementHandle.getHandle(), "SQLFetch()");
	return true;
}

} /* namespace database */
} /* namespace unixodbc4esl */
