#include "SKSEUtil/JSONUtil.h"
#include "SKSEUtil/StringUtil.h"

namespace SKSEUtil
{
	int compareJsonValues(const Json::Value& val1, const Json::Value& val2) {
		switch (val1.type()) {
		case Json::stringValue:
			return compareStringValue(val1.asCString(), val2);

		case Json::intValue:
			return compareIntValue(val1.asInt(), val2);

		case Json::uintValue:
			return compareUIntValue(val1.asUInt(), val2);

		case Json::realValue:
			return compareFloatValue(val1.asFloat(), val2);

		case Json::booleanValue:
			return compareBoolValue(val1.asBool(), val2);

		case Json::nullValue:
			return val2.isNull() ? 0 : -2;

		default:
			return -2;
		}
	}

	int compareStringValue(const std::string& compare, const Json::Value& comparing) {
		if (comparing.isString()) {
			int result = compare.compare(comparing.asCString());
			if (result < 0)
				return -1;
			else if (result > 0)
				return 1;
			return 0;
		}
		return -2;
	}

	int compareBoolValue(bool compare, const Json::Value& comparing) {
		bool comparing_val;

		switch (comparing.type()) {
		case Json::intValue:
			comparing_val = comparing.asInt() != 0;
			break;

		case Json::uintValue:
			comparing_val = comparing.asUInt() != 0;
			break;

		case Json::realValue:
			comparing_val = comparing.asFloat() != 0.0;
			break;

		case Json::booleanValue:
			comparing_val = comparing.asBool();
			break;

		default:
			return -2;
		}

		if (compare == comparing_val)
			return 0;
		else if (compare > comparing_val)
			return 1;
		else
			return -1;
	}

	int compareIntValue(int compare, const Json::Value& comparing) {
		int comparing_val;

		switch (comparing.type()) {
		case Json::intValue:
			comparing_val = comparing.asInt();
			break;

		case Json::uintValue:
			if (compare < 0 || comparing.asUInt() > INT_MAX)
				return -1;
			comparing_val = static_cast<int>(comparing.asUInt());
			break;

		case Json::realValue:
			comparing_val = static_cast<int>(comparing.asFloat());
			break;

		case Json::booleanValue:
			comparing_val = comparing.asBool() ? 1 : 0;
			break;

		default:
			return -2;
		}

		if (compare == comparing_val)
			return 0;
		else if (compare > comparing_val)
			return 1;
		else
			return -1;
	}

	int compareFloatValue(float compare, const Json::Value& comparing) {
		float comparing_val;

		switch (comparing.type()) {
		case Json::intValue:
			comparing_val = static_cast<float>(comparing.asInt());
			break;

		case Json::uintValue:
			if (compare < 0)
				return -1;
			comparing_val = static_cast<float>(comparing.asUInt());
			break;

		case Json::realValue:
			comparing_val = comparing.asFloat();
			break;

		case Json::booleanValue:
			comparing_val = comparing.asBool() ? 1.0f : 0.0f;
			break;

		default:
			return -2;
		}

		if (compare == comparing_val)
			return 0;
		else if (compare > comparing_val)
			return 1;
		else
			return -1;
	}

	int compareUIntValue(unsigned int compare, const Json::Value& comparing) {
		unsigned int comparing_val;

		switch (comparing.type()) {
		case Json::intValue:
			if (comparing.asInt() < 0)
				return 1;
			comparing_val = static_cast<unsigned int>(comparing.asInt());
			break;

		case Json::uintValue:
			comparing_val = comparing.asUInt();
			break;

		case Json::realValue:
			if (comparing.asFloat() < 0.0f)
				return 1;
			comparing_val = static_cast<unsigned int>(comparing.asFloat());
			break;

		case Json::booleanValue:
			comparing_val = comparing.asBool() ? 1u : 0u;
			break;

		default:
			return -2;
		}

		if (compare == comparing_val)
			return 0;
		else if (compare > comparing_val)
			return 1;
		else
			return -1;
	}

	bool isValueInValue(const Json::Value& val1, const Json::Value& val2) {
		if (val1.isString() && val2.isString()) {
			return containsSubstring(val1.asCString(), val2.asCString());
		}
		else if (val2.isArray()) {
			for (unsigned int i = 0u; i < val2.size(); i++) {
				if (compareJsonValues(val1, val2[i]) == 0)
					return true;
			}
			return false;
		}
		else if (val2.isObject() && val1.isString()) {
			std::string str1 = val1.asCString();
			for (Json::Value::const_iterator it = val2.begin(); it != val2.end(); ++it) {
				if (str1 == it.key().asCString())
					return true;
			}
			return false;
		}

		return false;
	}

	void jsonValueToString(const Json::Value& json_value, std::string& out) {
		if (json_value.isString())
			out = json_value.asCString();
		else {
			Json::FastWriter writer;
			out = stripEnds(writer.write(json_value),'\n');
		}
	}

	bool tryDeserializePoint3(const Json::Value& value, RE::NiPoint3& out) {
		if (value.isArray() && value.size() == 3u) {
			if (value[0].isNumeric() && value[1].isNumeric() && value[2].isNumeric()) {
				out.x = value[0].asFloat();
				out.y = value[1].asFloat();
				out.z = value[2].asFloat();
				return true;
			}
		}
		return false;
	}

	bool serializeJsonToRecord(SKSE::SerializationInterface* serde, const Json::Value& json_value) {
		Json::FastWriter writer;
		std::string value_str = writer.write(json_value);

		//Write size of string
		std::string::size_type value_str_size = std::strlen(value_str.c_str());
		serde->WriteRecordData(&value_str_size, sizeof(value_str_size));

		//Write string
		serde->WriteRecordData(value_str.c_str(), value_str_size);

		return true;
	}

	bool deserializeJsonFromRecord(SKSE::SerializationInterface* serde, Json::Value& json_value_out) {
		std::string::size_type value_str_size = 0u;
		serde->ReadRecordData(&value_str_size, sizeof(value_str_size));
		char* value_c_str = new char[value_str_size + 1];
		serde->ReadRecordData(value_c_str, value_str_size);
		value_c_str[value_str_size] = '\0';

		if (value_str_size == 0u)
			return false;

		Json::Reader reader;
		bool result = reader.parse(value_c_str, json_value_out);

		delete value_c_str;
		return result;
	}
}