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

#include <unixodbc4esl/database/BindVariable.h>
#include <unixodbc4esl/Logger.h>

#include <esl/Stacktrace.h>

namespace unixodbc4esl {
namespace database {

namespace {
Logger logger("unixodbc4esl::database::BindVariable");
}

BindVariable::BindVariable(std::size_t aLength)
: length(aLength),
  capacity(aLength)
{
	if(capacity > 0) {
		logger.trace << "BindVariable::BindVariable(" << length << ")\n";
		valueString = new char[capacity];
	}
}

BindVariable::~BindVariable() {
	if(capacity > 0) {
		delete[] valueString;
	}
}

void BindVariable::setLenght(std::size_t aLength) {
	if(length == aLength) {
		return;
	}

	length = aLength;
	logger.trace << "BindVariable::setLenght(" << length << ")\n";

	if(length > capacity) {
		if(capacity > 0) {
			delete[] valueString;
		}
		capacity = length;
		valueString = new char[capacity];
	}
}

std::size_t BindVariable::getLength() const noexcept {
	return length;
}

std::size_t BindVariable::getCapacity() const noexcept {
	return capacity;
}

void BindVariable::reset() {
	if(capacity > 0) {
		delete[] valueString;
		capacity = 0;
	}
	length = 0;
}

} /* namespace database */
} /* namespace unixodbc4esl */
