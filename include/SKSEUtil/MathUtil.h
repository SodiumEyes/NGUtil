#pragma once

#include "json/json.h"
#include <RE/Skyrim.h>
#include <random>

namespace SKSEUtil
{
	std::string hexToString(unsigned int value);
	unsigned int stringToHex(const std::string& value);
	bool stringToHex(const std::string& value, unsigned int* out);

	float smartModf(float dividend, float divisor);
	float radiansToDegrees(float radians);
	float degreesToRadians(float degrees);

	RE::NiPoint3 CameraPos();
	RE::NiPoint3 CameraDir();

	RE::NiPoint3 TransformedVector(const RE::NiPoint3& vec, const RE::NiMatrix3& transform);
	RE::NiPoint3 TransformedPoint(const RE::NiPoint3& p, const RE::NiTransform& transform);

	void serializePointToJson(const RE::NiPoint3& p, Json::Value& json_value);

};
