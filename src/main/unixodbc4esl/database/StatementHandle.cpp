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

#include <unixodbc4esl/database/StatementHandle.h>
#include <unixodbc4esl/database/Driver.h>
#include <unixodbc4esl/Logger.h>

#include <esl/database/exception/SqlError.h>
#include <esl/Stacktrace.h>
#include <esl/logging/Location.h>
#include <esl/logging/Logger.h>

#include <sqlext.h>

namespace unixodbc4esl {
namespace database {

namespace {
Logger logger("unixodbc4esl::database::PreparedHandle");
}

StatementHandle::StatementHandle(StatementHandle&& other)
: handle(other.handle)
{
	other.handle = SQL_NULL_HSTMT;
	logger.trace << "Statement handle constructed (moved)\n";
}

StatementHandle::StatementHandle(SQLHANDLE aHandle)
: handle(aHandle)
{
}

StatementHandle::~StatementHandle() {
	if(handle == SQL_NULL_HSTMT) {
		logger.debug << "Close statement handle (closed already)\n";
		return;
	}

	logger.debug << "Close statement handle\n";

	esl::logging::Location location;
	location.file = __FILE__;
	location.function = __func__;

	try {
		// free statement handle
		Driver::getDriver().freeHandle(*this);
	}
	catch (const esl::database::exception::SqlError& e) {
		ESL__LOGGER_WARN_THIS("esl::database::exception::SqlError exception occured\n");
		ESL__LOGGER_WARN_THIS(e.what(), "\n");
		location.line = __LINE__;
		e.getDiagnostics().dump(logger.warn, location);

		const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
		if(stacktrace) {
			location.line = __LINE__;
			stacktrace->dump(logger.warn, location);
		}
		else {
			ESL__LOGGER_WARN_THIS("no stacktrace\n");
		}
	}
	catch(const std::exception& e) {
		ESL__LOGGER_WARN_THIS("std::exception exception occured\n");
		ESL__LOGGER_WARN_THIS(e.what(), "\n");

		const esl::Stacktrace* stacktrace = esl::getStacktrace(e);
		if(stacktrace) {
			location.line = __LINE__;
			stacktrace->dump(logger.warn, location);
		}
		else {
			ESL__LOGGER_WARN_THIS("no stacktrace\n");
		}
	}
	catch (...) {
		ESL__LOGGER_ERROR_THIS("unkown exception occured\n");
	}

	handle = SQL_NULL_HSTMT;
}

StatementHandle& StatementHandle::operator=(StatementHandle&& other) {
	handle = other.handle;
	other.handle = SQL_NULL_HSTMT;
	logger.trace << "Statement handle moved\n";
	return *this;
}

StatementHandle::operator bool() const noexcept {
	return handle != SQL_NULL_HSTMT;
}

SQLHANDLE StatementHandle::getHandle() const noexcept {
	return handle;
}

} /* namespace database */
} /* namespace unixodbc4esl */
