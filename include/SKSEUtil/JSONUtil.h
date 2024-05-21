#pragma once

#include "json/json.h"
#include <RE/Skyrim.h>

namespace SKSEUtil
{
	int compareJsonValues(const Json::Value& val1, const Json::Value& val2);
	int compareStringValue(const std::string& compare, const Json::Value& comparing);
	int compareBoolValue(bool compare, const Json::Value& comparing);
	int compareIntValue(int compare, const Json::Value& comparing);
	int compareFloatValue(float compare, const Json::Value& comparing);
	int compareUIntValue(unsigned int compare, const Json::Value& comparing);

	bool isValueInValue(const Json::Value& val1, const Json::Value& val2);

	bool tryDeserializePoint3(const Json::Value& value, RE::NiPoint3& out);
}