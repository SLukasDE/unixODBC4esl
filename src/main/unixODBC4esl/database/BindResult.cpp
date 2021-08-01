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

#include <unixODBC4esl/database/BindResult.h>
#include <unixODBC4esl/database/Driver.h>
#include <unixODBC4esl/Logger.h>

#include <esl/Stacktrace.h>

namespace unixODBC4esl {
namespace database {

namespace {
Logger logger("unixODBC4esl::database::BindResult");
}

BindResult::BindResult(const StatementHandle& aStatementHandle, const esl::database::Column& aColumn, std::size_t aIndex)
: statementHandle(aStatementHandle),
  column(aColumn),
  index(aIndex)
{
	switch(column.getType()) {
	case esl::database::Column::Type::sqlInteger:
	case esl::database::Column::Type::sqlSmallInt:
		Driver::getDriver().bindCol(statementHandle, index, resultInteger, resultIndicator);
		break;

	case esl::database::Column::Type::sqlDouble:
	case esl::database::Column::Type::sqlNumeric:
	case esl::database::Column::Type::sqlDecimal:
	case esl::database::Column::Type::sqlFloat:
	case esl::database::Column::Type::sqlReal:
		Driver::getDriver().bindCol(statementHandle, index, resultDouble, resultIndicator);
		break;

	default:
		/*
		logger.trace << "Set buffer size for column[" << index << "]=\"" << column.getName() << "\" to " << column.getBufferSize() << " bytes.\n";
		if(column.getBufferSize() == 0) {
			valueData = nullptr;
			throw esl::addStacktrace(std::runtime_error("valueBufferSize == 0"));
		}

		valueData = new char[column.getBufferSize()+1];

		SQLLEN valueInputLength = column.getBufferSize();
		*/
		logger.trace << "BindResult:\n";
		logger.trace << "- resultIndicator: " << resultIndicator << "\n";
		//logger.trace << "- valueInputLength: " << valueInputLength << "\n";
		logger.trace << "- valueInputLength: " << resultDataSize << "\n";
		Driver::getDriver().bindCol(statementHandle, index, resultData, resultDataSize, resultIndicator);
		break;
	}

	if(logger.trace) {
		logger.trace << "Column " << index << ":\n";
		switch(column.getType()) {
		case esl::database::Column::Type::sqlBoolean:
			logger.trace << "    Type: sqlBoolean\n";
			break;
		case esl::database::Column::Type::sqlInteger:
			logger.trace << "    Type: sqlInteger\n";
			break;
		case esl::database::Column::Type::sqlSmallInt:
			logger.trace << "    Type: sqlSmallInt\n";
			break;
		case esl::database::Column::Type::sqlDouble:
			logger.trace << "    Type: sqlDouble\n";
			break;
		case esl::database::Column::Type::sqlNumeric:
			logger.trace << "    Type: sqlNumeric\n";
			break;
		case esl::database::Column::Type::sqlDecimal:
			logger.trace << "    Type: sqlDecimal\n";
			break;
		case esl::database::Column::Type::sqlFloat:
			logger.trace << "    Type: sqlFloat\n";
			break;
		case esl::database::Column::Type::sqlReal:
			logger.trace << "    Type: sqlReal\n";
			break;
		case esl::database::Column::Type::sqlVarChar:
			logger.trace << "    Type: sqlVarChar\n";
			break;
		case esl::database::Column::Type::sqlChar:
			logger.trace << "    Type: sqlChar\n";
			break;
		case esl::database::Column::Type::sqlDateTime:
			logger.trace << "    Type: sqlDateTime\n";
			break;
		case esl::database::Column::Type::sqlDate:
			logger.trace << "    Type: sqlDate\n";
			break;
		case esl::database::Column::Type::sqlTime:
			logger.trace << "    Type: sqlTime\n";
			break;
		case esl::database::Column::Type::sqlTimestamp:
			logger.trace << "    Type: sqlTimestamp\n";
			break;
		case esl::database::Column::Type::sqlWChar:
			logger.trace << "    Type: sqlWChar\n";
			break;
		case esl::database::Column::Type::sqlWVarChar:
			logger.trace << "    Type: sqlWVarChar\n";
			break;
		case esl::database::Column::Type::sqlWLongVarChar:
			logger.trace << "    Type: sqlWLongVarChar\n";
			break;
		default:
			logger.trace << "    Type: sqlUnknown\n";
			break;
		}
	}
}
/*
BindResult::~BindResult() {
	switch(column.getType()) {
	case esl::database::Column::Type::sqlInteger:
	case esl::database::Column::Type::sqlSmallInt:
	case esl::database::Column::Type::sqlDouble:
	case esl::database::Column::Type::sqlNumeric:
	case esl::database::Column::Type::sqlDecimal:
	case esl::database::Column::Type::sqlFloat:
	case esl::database::Column::Type::sqlReal:
		break;
	default:
		if(valueData) {
			delete[] valueData;
		}
		break;
	}
}
*/
void BindResult::setField(esl::database::Field& field) {
	if(isSqlNullData()) {
		logger.trace << "    Field: NULL\n";
		field = nullptr;
		return;
	}

	switch(column.getType()) {
	case esl::database::Column::Type::sqlInteger:
	case esl::database::Column::Type::sqlSmallInt:
		logger.trace << "    Field: Integer(" << resultInteger << ")\n";
		field = resultInteger;
		break;

	case esl::database::Column::Type::sqlDouble:
	case esl::database::Column::Type::sqlNumeric:
	case esl::database::Column::Type::sqlDecimal:
	case esl::database::Column::Type::sqlFloat:
	case esl::database::Column::Type::sqlReal:
		logger.trace << "    Field: Double(" << resultDouble << ")\n";
		field = resultDouble;
		break;

	case esl::database::Column::Type::sqlVarChar:
	case esl::database::Column::Type::sqlChar:
	default:
		logger.trace << "    Field: String preamble\n";
		logger.trace << "    - getResultLength() [0] = " << getResultDataLength() << "\n";
		//logger.trace << "    - bufferSize            = " << column.getBufferSize() << "\n";
		logger.trace << "    - bufferSize            = " << resultDataSize << "\n";

		// if(getResultLength() > column.getBufferSize()) {
		if(isSqlNoTotal()) {
			throw esl::addStacktrace(std::runtime_error("(1) getResultLength() == SQL_NO_TOTAL"));
#if 0
			std::string str;
			while(true) {
				if(isSqlNoTotal()) {
					throw esl::addStacktrace(std::runtime_error("(2) getResultLength() == SQL_NO_TOTAL"));
				}
				if(getResultLength() <= str.size()) {
					break;
				}

				Driver::getDriver().getData(statementHandle, static_cast<SQLUSMALLINT>(index+1),
						SQL_C_CHAR, valueData,
						column.getBufferSize(), &resultLength);

				if(isSqlNullData()) {
					throw esl::addStacktrace(std::runtime_error("Fetching of column \"" + std::to_string(index) + "\" was SQL_NO_TOTAL but getData() got SQL_NULL_DATA result."));
				}

				//std::string tmpStr;
				/*
				if(isNoTotal()) {
					tmpStr = std::string(valueData, column.getBufferSize());
					logger.trace << "               valueResultLength == SQL_NO_TOTAL\n";
				}
				else {*/
//				else if(getResultLength() > column.getBufferSize()) {
					if(getResultLength() < str.size()) {
						throw esl::addStacktrace(std::runtime_error("getResultLength() (=" + std::to_string(getResultLength()) + ") < str.size() (=" + std::to_string(str.size()) + ")."));
					}

					logger.trace << "               getResultLength() = " << getResultLength() << "\n";
					std::size_t length = getResultLength() - str.size();
					logger.trace << "               remaining length = " << length << "\n";
					if(length > column.getBufferSize()) {
						length = column.getBufferSize();
					}
//					if(length > 0) {
//						--length;
//					}

					logger.trace << "               fetch length = " << length << "\n";
					if(length > 0) {
						logger.trace << "               last byte is = " << (int) valueData[length-1] << "\n";
					}
					//tmpStr = std::string(valueData, length);
					str.append(valueData, length-1);
//				}
				/*
				else {
					tmpStr = std::string(valueData, getResultLength());
					logger.trace << "               valueResultLength != SQL_NO_TOTAL (" << getResultLength() << ")\n";
				}
				*/
				logger.trace << "               str += \"" << std::string(valueData, length) << "\"\n";
				//str += tmpStr;
			}

			logger.trace << "    Field: Str(\"" << str << "\")\n";
			field = str;
#endif
		}
		else if(getResultDataLength() > resultDataSize) {
			std::size_t tmpBufferSize = getResultDataLength();
			logger.trace << "    Field: String(...) [" << tmpBufferSize << "]\n";
			//char* tmpBuffer = new char[tmpBufferSize+1];
			std::vector<char> tmpBuffer(tmpBufferSize+1);

			Driver::getDriver().getData(statementHandle, static_cast<SQLUSMALLINT>(index+1),
					SQL_C_CHAR, &tmpBuffer[0], tmpBufferSize+1, &resultIndicator);

			if(isSqlNullData()) {
				//delete[] tmpBuffer;
				throw esl::addStacktrace(std::runtime_error("Fetching of column \"" + std::to_string(index) + "\" was SQL_NO_TOTAL but getData() got SQL_NULL_DATA result."));
			}

			std::string str(&tmpBuffer[0], tmpBufferSize);
			tmpBuffer.clear();
			//logger.trace << "    Field: Str(\"" << str << "\")\n";
			field = str;
		}
		else {
			std::string str(resultData, getResultDataLength());
			//logger.trace << "    Field: String(\"" << str << "\")\n";
			field = str;
		}
		break;
	}

}

std::size_t BindResult::getResultDataLength() const noexcept {
	return static_cast<std::size_t>(resultIndicator);
}

bool BindResult::isSqlNullData() const noexcept {
	return static_cast<SQLINTEGER>(resultIndicator) == SQL_NULL_DATA;
}

bool BindResult::isSqlNoTotal() const noexcept {
	return static_cast<SQLINTEGER>(resultIndicator) == SQL_NO_TOTAL;
}

} /* namespace database */
} /* namespace unixODBC4esl */
