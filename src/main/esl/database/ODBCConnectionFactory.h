#ifndef ESL_DATABASE_ODBCCONNECTIONFACTORY_H_
#define ESL_DATABASE_ODBCCONNECTIONFACTORY_H_

#include <esl/database/Connection.h>
#include <esl/database/ConnectionFactory.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace database {

class ODBCConnectionFactory : public ConnectionFactory {
public:
	struct Settings {
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		std::string connectionString;
		std::size_t defaultBufferSize = 65536;
		std::size_t maximumBufferSize = 65536;
	};

	ODBCConnectionFactory(const Settings& settings);

	static std::unique_ptr<ConnectionFactory> create(const std::vector<std::pair<std::string, std::string>>& settings);

    std::unique_ptr<Connection> createConnection() override;

private:
	std::unique_ptr<ConnectionFactory> connectionFactory;
};

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_DATABASE_ODBCCONNECTIONFACTORY_H_ */
