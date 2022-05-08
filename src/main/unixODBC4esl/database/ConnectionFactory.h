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

#ifndef UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_
#define UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_

#include <esl/database/Interface.h>
#include <esl/database/Connection.h>
#include <esl/object/Interface.h>

#include <sqlext.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace unixODBC4esl {
namespace database {

class ConnectionFactory : public esl::database::Interface::ConnectionFactory {
public:
	static inline const char* getImplementation() {
		return "unixODBC4esl";
	}

	static std::unique_ptr<esl::database::Interface::ConnectionFactory> createConnectionFactory(const std::vector<std::pair<std::string, std::string>>& settings);

	ConnectionFactory(const std::vector<std::pair<std::string, std::string>>& settings);
	~ConnectionFactory();

	SQLHANDLE getHandle() const;

	std::unique_ptr<esl::database::Connection> createConnection() override;

	void setConnectionString(std::string connectionString);

	/* this is the buffer size for fetching values from columns if their real buffer size cannot be figured out */
	void setDefaultBufferSize(const std::string& defaultBufferSizeString);

	/* this is the maximum buffer size for fetching values if their real buffer size is bigger. A value of zero is unlimited.
	 * Warning: don't set this value to zero because e.g. BLOB-Fields currently say they need to take 2GB */
	void setMaximumBufferSize(const std::string& maximumBufferSizeString);

private:
	SQLHANDLE handle;

	std::string connectionString;
	std::size_t defaultBufferSize = 65536;
	std::size_t maximumBufferSize = 65536;
};

} /* namespace database */
} /* namespace unixODBC4esl */

#endif /* UNIXODBC4ESL_DATABASE_CONNECTIONFACTORY_H_ */
