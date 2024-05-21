#include "SKSEUtil/MathUtil.h"
#include <numbers>

using namespace RE;

namespace SKSEUtil
{
	std::string hexToString(unsigned int value) {
		return (std::stringstream() << std::hex << std::uppercase << value).str();
	}

	unsigned int stringToHex(const std::string& value) {
		std::istringstream st(value);
		unsigned int hex = 0u;
		st >> std::hex >> hex;
		return hex;
	}

	float smartModf(float dividend, float divisor)
	{
		if (dividend < 0.0f)
			dividend -= divisor * std::floor((dividend / divisor) - 1.0f);

		return std::fmod(dividend, divisor);
	}

	float radiansToDegrees(float radians) {
		return radians * static_cast<float>(std::numbers::inv_pi) * 180.0f;
	}

	float degreesToRadians(float degrees) {
		return (degrees / 180.0f) * static_cast<float>(std::numbers::pi);
	}

	RE::NiPoint3 CameraPos()
	{
		return PlayerCamera::GetSingleton()->cameraRoot->world.translate;
	}

	RE::NiPoint3 CameraDir()
	{
		return TransformedVector(NiPoint3(0.0f, 1.0f, 0.0f), PlayerCamera::GetSingleton()->cameraRoot->world.rotate);
	}

	RE::NiPoint3 TransformedVector(const NiPoint3& vec, const NiMatrix3& transform)
	{
		return NiPoint3(
			vec.x * transform.entry[0][0] + vec.y * transform.entry[0][1] + vec.z * transform.entry[0][2],
			vec.x * transform.entry[1][0] + vec.y * transform.entry[1][1] + vec.z * transform.entry[1][2],
			vec.x * transform.entry[2][0] + vec.y * transform.entry[2][1] + vec.z * transform.entry[2][2]);
	}

	RE::NiPoint3 TransformedPoint(const RE::NiPoint3& p, const RE::NiTransform& transform) {
		return (TransformedVector(p, transform.rotate) * transform.scale) + transform.translate;
	}

	void serializePointToJson(const RE::NiPoint3& p, Json::Value& json_value)
	{
		json_value.clear();
		json_value[0] = p.x;
		json_value[1] = p.y;
		json_value[2] = p.z;
	}

}
