#pragma once

#include "json/json.h"
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace SKSEUtil
{
	int compareJsonValues(const Json::Value& val1, const Json::Value& val2);
	int compareStringValue(const std::string& compare, const Json::Value& comparing);
	int compareBoolValue(bool compare, const Json::Value& comparing);
	int compareIntValue(int compare, const Json::Value& comparing);
	int compareFloatValue(float compare, const Json::Value& comparing);
	int compareUIntValue(unsigned int compare, const Json::Value& comparing);

	bool isValueInValue(const Json::Value& val1, const Json::Value& val2);
	void jsonValueToString(const Json::Value& json_value, std::string& out);

	bool tryDeserializePoint3(const Json::Value& value, RE::NiPoint3& out);

	bool serializeJsonToRecord(SKSE::SerializationInterface* serde, const Json::Value& json_value);
	bool deserializeJsonFromRecord(SKSE::SerializationInterface* serde, Json::Value& json_value_out);
}