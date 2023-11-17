/*
 * This file is part of unixODBC4esl.
 * Copyright (C) 2020-2023 Sven Lukas
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

#ifndef UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_
#define UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_

#include <esl/database/Connection.h>
#include <esl/database/ConnectionFactory.h>
#include <esl/database/ODBCConnectionFactory.h>

#include <sqlext.h>

#include <memory>

namespace unixODBC4esl {
inline namespace v1_6 {
namespace database {

class ConnectionFactory : public esl::database::ConnectionFactory {
public:
	ConnectionFactory(esl::database::ODBCConnectionFactory::Settings settings);
	~ConnectionFactory();

	const esl::database::ODBCConnectionFactory::Settings& getSettings() const noexcept;
	SQLHANDLE getHandle() const;

	std::unique_ptr<esl::database::Connection> createConnection() override;

private:
	esl::database::ODBCConnectionFactory::Settings settings;
	SQLHANDLE handle;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace unixODBC4esl */

#endif /* UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_ */
