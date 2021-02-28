#if 0
#include <esl/logging/Logger.h>
#include <esl/logging/Level.h>
#include <esl/logging/Layout.h>
#include <esl/logging/appender/OStream.h>
#include <esl/logging/appender/MemBuffer.h>
#include <esl/database/exception/SqlError.h>
#include <esl/database/Diagnostics.h>
#include <esl/database/ConnectionFactory.h>
#include <esl/database/Connection.h>

#include <odbc4esl/Module.h>
#include <odbc4esl/Logger.h>
#include <logbook4esl/Module.h>

#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include <stdexcept>

namespace {
esl::logging::Layout layout;
esl::logging::appender::OStream appenderOstream(std::cout, std::cout, std::cout, std::cout, std::cout);

esl::database::Connection& getConnection() {
	static esl::database::ConnectionFactory connectionFactory("odbc4esl");

	connectionFactory.addSetting("driver", "/usr/lib/oracle/19.6/client64/lib/libsqora.so.19.1");
	connectionFactory.addSetting("connectionString", "DBQ=ORA-T-DATART-KOE.dc.rewe.local:1521/DATART;UID=DATARUNNER;PWD=g4fgserd;APA=T;");

	static std::unique_ptr<esl::database::Connection> connection = connectionFactory.createConnection();
	if(!connection) {
		throw std::runtime_error("no connection");
	}
	return *connection;
}

odbc4esl::Logger logger("odbc4esl::cgi");
}

int main(int argc, char* argv[]) {
	esl::getModule().addInterfaces(logbook4esl::getModule());
	esl::getModule().addInterfaces(odbc4esl::getModule());

	layout.addSetting("showTimestamp", "false");
	layout.addSetting("showLevel",     "true" );
	layout.addSetting("showTypeName",  "true" );
	layout.addSetting("showAddress",   "false");
	layout.addSetting("showFile",      "true" );
	layout.addSetting("showFunction",  "true" );
	layout.addSetting("showLineNo",    "true" );
	layout.addSetting("showThreadNo",  "false");

	esl::logging::addAppender(appenderOstream);

	appenderOstream.setRecordLevel();
	appenderOstream.setLayout(&layout);

	esl::logging::setLevel(esl::logging::Level::TRACE, "*");
	esl::logging::setLevel(esl::logging::Level::WARN, "esl::*");

	try {
		std::string statementString = "INSERT INTO DATARUNNER.HOST_NODE (NODE_ID, HOST) VALUES ('xxx', 'xxx');";
//		std::string statementString = "DELETE FROM DATARUNNER.SCENARIO WHERE SCENARIO_NAME='/xxx/';";
		esl::database::Connection& connection = getConnection();

		esl::database::PreparedStatement statement(connection.prepare(statementString));
		statement.execute();

		connection.commit();
		logger.error << "Executed: \"" << statementString << "\"\n";
	}
	catch(const esl::database::exception::SqlError& sqlError) {
		std::cout << "esl::database::exception::SqlError" << std::endl;
		std::cout << "what() = " << sqlError.what() << std::endl;
		std::cout << "sqlReturnCode = " << sqlError.getSqlReturnCode() << std::endl;
		sqlError.getDiagnostics().dump(logger.error);
		return 1;
	}
	catch(const std::runtime_error& e) {
		std::cout << "std::runtime_error" << std::endl;
		std::cout << "what() = " << e.what() << std::endl;
		return 1;
	}
	catch(const std::exception& e) {
		std::cout << "std::exception" << std::endl;
		std::cout << "what() = " << e.what() << std::endl;
		return 1;
	}
	catch(...) {
		std::cout << "(exception)" << std::endl;
		return 1;
	}

	return 0;
}
#endif

