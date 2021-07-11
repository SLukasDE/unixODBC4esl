/*
 * This file is part of unixODBC4esl.
 * Copyright (C) 2021 Sven Lukas
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

#include <unixODBC4esl/Module.h>
#include <unixODBC4esl/database/ConnectionFactory.h>

#include <esl/object/Interface.h>
#include <esl/database/Interface.h>
#include <esl/Module.h>

namespace unixODBC4esl {

void Module::install(esl::module::Module& module) {
	esl::setModule(module);

	module.addInterface(esl::object::Interface::createInterface(
			database::ConnectionFactory::getImplementation(),
			&database::ConnectionFactory::createObject));

	module.addInterface(esl::database::Interface::createInterface(
			database::ConnectionFactory::getImplementation(),
			&database::ConnectionFactory::createConnectionFactory));
}

} /* namespace unixODBC4esl */
