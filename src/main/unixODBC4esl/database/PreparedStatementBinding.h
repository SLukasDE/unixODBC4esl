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

#ifndef UNIXODBC4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_
#define UNIXODBC4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_

#include <unixODBC4esl/database/Connection.h>
#include <unixODBC4esl/database/StatementHandle.h>

#include <esl/database/PreparedStatement.h>
#include <esl/database/Column.h>
#include <esl/database/Field.h>

#include <string>
#include <vector>

namespace unixODBC4esl {
inline namespace v1_6 {
namespace database {

class PreparedStatementBinding : public esl::database::PreparedStatement::Binding {
public:
	PreparedStatementBinding(const Connection& connection, const std::string& sql, std::size_t defaultBufferSize, std::size_t maximumBufferSize);

	const std::vector<esl::database::Column>& getParameterColumns() const override;
	const std::vector<esl::database::Column>& getResultColumns() const override;
	esl::database::ResultSet execute(const std::vector<esl::database::Field>& fields) override;
	void* getNativeHandle() const override;

private:
	const Connection& connection;
	std::string sql;
	StatementHandle statementHandle;
	std::vector<esl::database::Column> parameterColumns;
	std::vector<esl::database::Column> resultColumns;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace unixODBC4esl */

#endif /* UNIXODBC4ESL_DATABASE_PREPAREDSTATEMENTBINDING_H_ */
