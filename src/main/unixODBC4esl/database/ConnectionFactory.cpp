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

#include <unixODBC4esl/database/ConnectionFactory.h>
#include <unixODBC4esl/database/Connection.h>
#include <unixODBC4esl/database/Driver.h>
#include <unixODBC4esl/Logger.h>

#include <esl/logging/Location.h>
#include <esl/database/exception/SqlError.h>
#include <esl/Stacktrace.h>

#include <stdexcept>

namespace unixODBC4esl {
namespace database {

namespace {
Logger logger("unixODBC4esl::database::ConnectionFactory");
}

std::unique_ptr<esl::object::Interface::Object> ConnectionFactory::createObject(const esl::database::Interface::Settings& settings) {
	return std::unique_ptr<esl::object::Interface::Object>(new ConnectionFactory(settings));
}

std::unique_ptr<esl::database::Interface::ConnectionFactory> ConnectionFactory::createConnectionFactory(const esl::database::Interface::Settings& settings) {
	return std::unique_ptr<esl::database::Interface::ConnectionFactory>(new ConnectionFactory(settings));
}

ConnectionFactory::ConnectionFactory(const esl::database::Interface::Settings& settings)
: esl::database::Interface::ConnectionFactory(),
  handle(Driver::getDriver().allocHandleEnvironment())
{
  	// switch to ODBC 3.0
  	Driver::getDriver().setEnvAttr(*this, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

	bool hasConnectionString = false;

	for(const auto& setting : settings) {
		if(setting.first == "connectionString") {
			connectionString = setting.second;
			hasConnectionString = true;
		}
		else if(setting.first == "defaultBufferSize") {
			defaultBufferSize = std::stoi(setting.second);
		}
		else if(setting.first == "maximumBufferSize") {
			maximumBufferSize = std::stoi(setting.second);
		}
		else {
			throw esl::addStacktrace(std::runtime_error("Key \"" + setting.first + "\" is unknown"));
		}
	}

	if(hasConnectionString == false) {
		throw esl::addStacktrace(std::runtime_error("Key \"connectionString\" is missing"));
	}
}

ConnectionFactory::~ConnectionFactory() {
	esl::logging::Location location;
	location.function = __func__;
	location.file = __FILE__;

	try {
		Driver::getDriver().freeHandle(*this);
		handle = SQL_NULL_HENV;
	}
	catch (const esl::database::exception::SqlError& e) {
		ESL__LOGGER_WARN("esl::database::exception::SqlError exception occured\n");
		ESL__LOGGER_WARN(e.what(), "\n");

		location.line = __LINE__;
    	e.getDiagnostics().dump(logger.warn, location);

		const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
    	if(stacktrace) {
    		location.line = __LINE__;
    		stacktrace->dump(logger.warn, location);
    	}
    	else {
    		ESL__LOGGER_WARN("no stacktrace\n");
    	}
	}
	catch(const std::exception& e) {
		ESL__LOGGER_WARN("std::exception exception occured\n");
		ESL__LOGGER_WARN(e.what(), "\n");

		const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
    	if(stacktrace) {
    		location.line = __LINE__;
    		stacktrace->dump(logger.warn, location);
    	}
    	else {
    		ESL__LOGGER_WARN("no stacktrace\n");
    	}
	}
	catch (...) {
		ESL__LOGGER_ERROR("unkown exception occured\n");
	}
}

SQLHANDLE ConnectionFactory::getHandle() const {
	return handle;
}

std::unique_ptr<esl::database::Connection> ConnectionFactory::createConnection() {
	return std::unique_ptr<esl::database::Connection>(new Connection(*this, connectionString, defaultBufferSize, maximumBufferSize));
}

} /* namespace database */
} /* namespace unixODBC4esl */
