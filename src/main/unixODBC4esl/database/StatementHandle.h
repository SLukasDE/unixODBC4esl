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

#ifndef UNIXODBC4ESL_DATABASE_STATEMENTHANDLE_H_
#define UNIXODBC4ESL_DATABASE_STATEMENTHANDLE_H_

#include <sqlext.h>

namespace unixODBC4esl {
namespace database {

class StatementHandle {
public:
	StatementHandle() = default;
	StatementHandle(const StatementHandle&) = delete;
	StatementHandle(StatementHandle&& statementHandle);
	StatementHandle(SQLHANDLE handle);

	~StatementHandle();

	StatementHandle& operator=(const StatementHandle&) = delete;
	StatementHandle& operator=(StatementHandle&& other);

	explicit operator bool() const noexcept;

	SQLHANDLE getHandle() const noexcept;

protected:
	SQLHANDLE handle = SQL_NULL_HANDLE;
};

} /* namespace database */
} /* namespace unixODBC4esl */

#endif /* UNIXODBC4ESL_DATABASE_STATEMENTHANDLE_H_ */
