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

#ifndef UNIXODBC4ESL_DATABASE_CONNECTION_H_
#define UNIXODBC4ESL_DATABASE_CONNECTION_H_

#include <unixODBC4esl/database/ConnectionFactory.h>

#include <esl/database/Connection.h>
#include <esl/database/PreparedStatement.h>
#include <esl/database/ResultSet.h>

#include <sqlext.h>

#include <string>
#include <vector>

namespace unixODBC4esl {
namespace database {

class Connection : public esl::database::Connection {
public:
	Connection(const ConnectionFactory& connectionFactory, const std::string& connectionString, std::size_t defaultBufferSize, std::size_t maximumBufferSize);
	~Connection();

	SQLHANDLE getHandle() const;

	esl::database::PreparedStatement prepare(const std::string& sql) const override;
	esl::database::ResultSet getTable(const std::string& tableName);

	void commit() const override;
	void rollback() const override;
	bool isClosed() const override;

	void* getNativeHandle() const override;

private:
	SQLHANDLE handle;
	std::size_t defaultBufferSize;
	std::size_t maximumBufferSize;
};

} /* namespace database */
} /* namespace unixODBC4esl */

#endif /* UNIXODBC4ESL_DATABASE_CONNECTION_H_ */
