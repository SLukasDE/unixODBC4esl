/*
 * This file is part of odbc4esl.
 * Copyright (C) 2020-2023 Sven Lukas
 *
 * Odbc4esl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Odbc4esl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with mhd4esl.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <odbc4esl/database/StatementHandle.h>
#include <odbc4esl/database/Driver.h>

#include <esl/Logger.h>

#include <esl/database/exception/SqlError.h>
#include <esl/system/Stacktrace.h>
#include <esl/monitoring/Streams.h>

namespace odbc4esl {
inline namespace v1_6 {
namespace database {

namespace {
esl::Logger logger("odbc4esl::database::PreparedHandle");
}

StatementHandle::StatementHandle(StatementHandle&& other)
: handle(other.handle)
{
	other.handle = SQL_NULL_HSTMT;
	logger.trace << "Statement handle constructed (moved)\n";
}

StatementHandle::StatementHandle(SQLHANDLE aHandle)
: handle(aHandle)
{ }

StatementHandle::~StatementHandle() {
	if(handle == SQL_NULL_HSTMT) {
		return;
	}

	logger.trace << "Close statement handle\n";

	esl::monitoring::Streams::Location location;
	location.file = __FILE__;
	location.function = __func__;

	try {
		// free statement handle
		Driver::getDriver().freeHandle(*this);
		handle = SQL_NULL_HSTMT;
	}
	catch (const esl::database::exception::SqlError& e) {
		ESL__LOGGER_WARN_THIS("esl::database::exception::SqlError exception occured\n");
		ESL__LOGGER_WARN_THIS(e.what(), "\n");
		location.line = __LINE__;
		e.getDiagnostics().dump(logger.warn, location);

		const esl::system::Stacktrace* stacktrace = esl::system::Stacktrace::get(e);
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

		const esl::system::Stacktrace* stacktrace = esl::system::Stacktrace::get(e);
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
} /* inline namespace v1_6 */
} /* namespace odbc4esl */
