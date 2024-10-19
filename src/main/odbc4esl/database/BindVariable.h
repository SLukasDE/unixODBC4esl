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

#ifndef ODBC4ESL_DATABASE_BINDVARIABLE_H_
#define ODBC4ESL_DATABASE_BINDVARIABLE_H_

#include <odbc4esl/database/StatementHandle.h>

#include <esl/database/Field.h>
#include <esl/database/Column.h>

#include <sqlext.h>

#include <string>
#include <cstdint>
#include <cstddef>

namespace odbc4esl {
inline namespace v1_6 {
namespace database {

struct BindVariable {
	BindVariable(BindVariable&& other) = delete;
	BindVariable(const StatementHandle& statementHandle, const esl::database::Column& column, std::size_t index);
	~BindVariable();

	BindVariable& operator=(const BindVariable&) = delete;
	BindVariable& operator=(BindVariable&& other) = delete;

	void getField(const esl::database::Field& field);

private:
	const StatementHandle& statementHandle;
	const esl::database::Column& column;
	const std::size_t index;

	union {
		char* valueString;
		std::int64_t valueInteger;
		double valueDouble;
	};

	mutable SQLLEN resultLength = 0;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace odbc4esl */

#endif /* ODBC4ESL_DATABASE_BINDVARIABLE_H_ */
