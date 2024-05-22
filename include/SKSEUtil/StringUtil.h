#pragma once

#include <string>
#include <RE/Skyrim.h>

namespace SKSEUtil
{
	std::string toLowercaseString(const std::string& str);
	std::string toUppercaseString(const std::string& str);

	size_t findSubstring(const std::string& str, const std::string& substr, size_t offset = 0u);
	bool containsSubstring(const std::string& str, const std::string& substr, size_t offset = 0u);
	size_t findAnyChar(const std::string& str, const std::string& chars, size_t offset = 0u);
	bool nonCaseSensitiveEquals(const std::string& str1, const std::string& str2);
	std::string stripEnds(const std::string& str, char c);
	unsigned int splitString(const std::string& str, char delim, std::vector<std::string>& out);

	template <typename Type>
	bool stringToValue(const std::string& str, Type& dest) {
		std::istringstream stream(str);
		stream >> dest;
		return !stream.fail();
	}
};
