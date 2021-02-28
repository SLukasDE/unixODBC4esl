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

#ifndef UNIXODBC4ESL_LOGGER_H_
#define UNIXODBC4ESL_LOGGER_H_

#include <esl/logging/Logger.h>
#include <esl/logging/Level.h>

namespace unixodbc4esl {

#ifdef UNIXODBC4ESL_LOGGING_LEVEL_DEBUG
using Logger = esl::logging::Logger<esl::logging::Level::TRACE>;
#else
using Logger = esl::logging::Logger<esl::logging::Level::WARN>;
#endif

} /* namespace unixodbc4esl */

#endif /* UNIXODBC4ESL_LOGGER_H_ */
