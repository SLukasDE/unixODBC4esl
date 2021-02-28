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

#ifndef UNIXODBC4ESL_DATABASE_BINDVARIABLE_H_
#define UNIXODBC4ESL_DATABASE_BINDVARIABLE_H_

#include <sqlext.h>

#include <string>
#include <cstdint>
#include <cstddef>

namespace unixodbc4esl {
namespace database {

struct BindVariable {
	BindVariable() = default;
	BindVariable(std::size_t length);
	~BindVariable();

	void setLenght(std::size_t length);
	std::size_t getLength() const noexcept;
	std::size_t getCapacity() const noexcept;

	void reset();

	SQLLEN valueResultLength = 0;

	union {
		char* valueString;
		bool valueBoolean;
		std::int64_t valueInteger;
		double valueDouble;
	};

private:
	/* current length that has been set by setLength */
	std::size_t length = 0;

	/* size of the allocated memory (capacity >= length) */
	std::size_t capacity = 0;
};

} /* namespace database */
} /* namespace unixodbc4esl */

#endif /* UNIXODBC4ESL_DATABASE_BINDVARIABLE_H_ */
