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

#include <unixodbc4esl/database/ConnectionFactory.h>
#include <unixodbc4esl/database/Connection.h>
#include <unixodbc4esl/database/Driver.h>
#include <unixodbc4esl/Logger.h>

#include <esl/logging/Location.h>
#include <esl/database/exception/SqlError.h>
#include <esl/Stacktrace.h>

#include <sqlext.h>

#include <stdexcept>

namespace unixodbc4esl {
namespace database {

namespace {
Logger logger("unixodbc4esl::database::ConnectionFactory");
}

std::unique_ptr<esl::database::Interface::ConnectionFactory> ConnectionFactory::create(const esl::object::Values<std::string>& settings) {
	return std::unique_ptr<esl::database::Interface::ConnectionFactory>(new ConnectionFactory(settings));
}

std::unique_ptr<esl::object::Interface::Object> ConnectionFactory::createObject() {
	esl::object::Values<std::string> settings;
	return std::unique_ptr<esl::object::Interface::Object>(new ConnectionFactory(settings));
}

ConnectionFactory::ConnectionFactory(const esl::object::Values<std::string>& settings)
: esl::database::Interface::ConnectionFactory(),
  handle(Driver::getDriver().allocHandleEnvironment())
{
  	// switch to ODBC 3.0
  	Driver::getDriver().setEnvAttr(*this, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);

	for(const auto setting : settings.getValues()) {
		addSetting(setting.first, setting.second);
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

void ConnectionFactory::addSetting(const std::string& key, const std::string& value) {
	if(key == "connectionString") {
		setConnectionString(value);
	}
	else if(key == "defaultBufferSize") {
		setDefaultBufferSize(value);
	}
	else if(key == "maximumBufferSize") {
		setMaximumBufferSize(value);
	}
	else {
		esl::object::Settings::addSetting(key, value);
	}
}

void ConnectionFactory::setConnectionString(std::string aConnectionString) {
	connectionString = std::move(aConnectionString);
}

void ConnectionFactory::setDefaultBufferSize(const std::string& defaultBufferSizeString) {
	defaultBufferSize = std::stoi(defaultBufferSizeString);
}

void ConnectionFactory::setMaximumBufferSize(const std::string& maximumBufferSizeString) {
	maximumBufferSize = std::stoi(maximumBufferSizeString);
}

} /* namespace database */
} /* namespace unixodbc4esl */
