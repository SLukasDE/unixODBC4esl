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

#ifndef ODBC4ESL_DATABASE_RESULTSETBINDING_H_
#define ODBC4ESL_DATABASE_RESULTSETBINDING_H_

#include <odbc4esl/database/StatementHandle.h>
#include <odbc4esl/database/BindResult.h>
#include <odbc4esl/database/BindVariable.h>

#include <esl/database/ResultSet.h>
#include <esl/database/Column.h>
#include <esl/database/Field.h>

#include <vector>

namespace odbc4esl {
inline namespace v1_6 {
namespace database {

class Environment;

class ResultSetBinding : public esl::database::ResultSet::Binding {
public:
	ResultSetBinding(StatementHandle&& statementHandle, const std::vector<esl::database::Column>& resultColumns);

	bool fetch(std::vector<esl::database::Field>& fields) override;
	bool isEditable(std::size_t columnIndex) override;
	void add(std::vector<esl::database::Field>& fields) override;
	void save(std::vector<esl::database::Field>& fields) override;

private:
	StatementHandle statementHandle;
	std::vector<std::unique_ptr<BindResult>> bindResult;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace odbc4esl */

#endif /* ODBC4ESL_DATABASE_RESULTSETBINDING_H_ */
