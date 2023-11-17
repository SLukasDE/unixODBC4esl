#include <esl/database/ODBCConnectionFactory.h>

#include <unixODBC4esl/database/ConnectionFactory.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace database {

ODBCConnectionFactory::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasDefaultBufferSize = false;
	bool hasMaximumBufferSize = false;

	for(const auto& setting : settings) {
		if(setting.first == "connection-string" || setting.first == "connectionString") {
			if(!connectionString.empty()) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" at ODBCConnectionFactory");
			}
			connectionString = setting.second;
			if(connectionString.empty()) {
				throw std::runtime_error("Invalid value \"\" for parameter key \"" + setting.first + "\" at ODBCConnectionFactory");
			}
		}
		else if(setting.first == "default-buffer-size") {
			if(hasDefaultBufferSize) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" at ODBCConnectionFactory");
			}
			hasDefaultBufferSize = true;
			defaultBufferSize = std::stoi(setting.second);
		}
		else if(setting.first == "maximum-buffer-size") {
			if(hasMaximumBufferSize) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" at ODBCConnectionFactory");
			}
			hasMaximumBufferSize = true;
			maximumBufferSize = std::stoi(setting.second);
		}
		else {
			throw std::runtime_error("Key \"" + setting.first + "\" is unknown");
		}
	}

	if(connectionString.empty()) {
		throw std::runtime_error("Key \"connection-string\" is missing");
	}
}

ODBCConnectionFactory::ODBCConnectionFactory(const Settings& settings)
: connectionFactory(new unixODBC4esl::database::ConnectionFactory(settings))
{ }

std::unique_ptr<ConnectionFactory> ODBCConnectionFactory::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<ConnectionFactory>(new ODBCConnectionFactory(Settings(settings)));
}

std::unique_ptr<Connection> ODBCConnectionFactory::createConnection() {
	return connectionFactory->createConnection();
}

} /* namespace database */
} /* inline namespace v1_6 */
} /* namespace esl */
