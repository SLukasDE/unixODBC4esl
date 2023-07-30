/*
 * This file is part of unixODBC4esl.
 * Copyright (C) 2020-2023 Sven Lukas
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

#include <unixODBC4esl/Plugin.h>
#include <unixODBC4esl/database/ConnectionFactory.h>

#include <esl/database/ConnectionFactory.h>

namespace unixODBC4esl {
inline namespace v1_6 {

void Plugin::install(esl::plugin::Registry& registry, const char* data) {
	esl::plugin::Registry::set(registry);

	registry.addPlugin<esl::database::ConnectionFactory>(
			"unixODBC4esl/database/ConnectionFactory",
			&database::ConnectionFactory::create);
}

} /* inline namespace v1_6 */
} /* namespace unixODBC4esl */
