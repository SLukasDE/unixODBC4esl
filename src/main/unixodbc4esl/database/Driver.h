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

#ifndef UNIXODBC4ESL_DATABASE_DRIVER_H_
#define UNIXODBC4ESL_DATABASE_DRIVER_H_

#include <unixodbc4esl/database/ConnectionFactory.h>
#include <unixodbc4esl/database/Connection.h>
#include <unixodbc4esl/database/StatementHandle.h>

#include <esl/database/Column.h>
#include <esl/database/Diagnostic.h>

#include <sqlext.h>

#include <string>

namespace unixodbc4esl {
namespace database {

class Driver {
public:
	static const Driver& getDriver();

	static esl::database::Column::Type sqlType2ColumnType(SQLSMALLINT sqlType);
	static SQLSMALLINT columnType2SqlType(esl::database::Column::Type columnType);

	SQLHANDLE allocHandleEnvironment() const;
	SQLHANDLE allocHandleConnection(const ConnectionFactory& connectionFactory) const;
	void freeHandle(const ConnectionFactory& connectionFactory) const;
	void freeHandle(const Connection& connection) const;
	void freeHandle(const StatementHandle& statementHandle) const;

	void setEnvAttr(const ConnectionFactory& connectionFactory, SQLINTEGER attribute, SQLPOINTER value, SQLINTEGER stringLength) const;

	void setConnectAttr(const Connection& connection, SQLINTEGER attribute, SQLPOINTER value, SQLINTEGER stringLength) const;
	void driverConnect(const Connection& connection, const std::string connectionString) const;
	void endTran(const Connection& connection, SQLSMALLINT type) const;
	void disconnect(const Connection& connection) const;
	bool getDiagRec(esl::database::Diagnostic& diagnostic, SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT index) const;
	StatementHandle prepare(const Connection& connection, const std::string& sql) const;
	SQLSMALLINT numResultCols(const StatementHandle& statementHandle) const;
	SQLSMALLINT numParams(const StatementHandle& statementHandle) const;
	void describeCol(const StatementHandle& statementHandle, SQLSMALLINT index, std::string& resultColumnName, esl::database::Column::Type& resultColumnType, std::size_t& resultCharacterLength, std::size_t& resultDecimalDigits, bool& resultNullable) const;
	void colAttributeDisplaySize(const StatementHandle& statementHandle, SQLSMALLINT index, std::size_t& resultDisplayLength) const;
	void describeParam(const StatementHandle& statementHandle, SQLSMALLINT index, esl::database::Column::Type& resultColumnType, std::size_t& resultCharacterLength, std::size_t& resultDecimalDigits, bool& resultNullable) const;
	void bindParameter(const StatementHandle& statementHandle, SQLSMALLINT index,
			SQLSMALLINT        paramType,
			SQLSMALLINT        dataType,
			SQLSMALLINT        sqlType,
			const esl::database::Column& column,
			SQLPOINTER         rgbValue,
			SQLLEN             cbValueMax,
			SQLLEN*            pcbValue) const;
	void bindCol(const StatementHandle& statementHandle, SQLSMALLINT index,
			SQLSMALLINT       dataType,
			SQLPOINTER        rgbValue,
			SQLLEN            cbValueMax,
			SQLLEN*           pcbValue) const;
	void getData(const StatementHandle& statementHandle, SQLSMALLINT index,
			SQLSMALLINT       dataType,
			void*             dataValue,
			std::size_t       dataBufferLength,
			SQLLEN*           dataButterLengthOrIndicator) const;

	void execute(const StatementHandle& statementHandle) const;
	bool fetch(const StatementHandle& statementHandle) const;
};

} /* namespace database */
} /* namespace unixodbc4esl */

#endif /* UNIXODBC4ESL_DATABASE_DRIVER_H_ */
