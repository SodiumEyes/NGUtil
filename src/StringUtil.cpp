#include "SKSEUtil/StringUtil.h"

using namespace RE;

namespace SKSEUtil
{
#define str_npos std::string::npos

	std::string toLowercaseString(const std::string& str) {
		std::string out;
		out.resize(str.size());
		for (std::string::size_type i = 0u; i < str.size(); i++)
			out[i] = static_cast<char>(std::tolower(str[i]));
		return out;
	}

	std::string toUppercaseString(const std::string& str) {
		std::string out;
		out.resize(str.size());
		for (std::string::size_type i = 0u; i < str.size(); i++)
			out[i] = static_cast<char>(std::toupper(str[i]));
		return out;
	}

	std::size_t findSubstring(const std::string& str, const std::string& substr, size_t offset) {

		if (substr.empty() || substr.size() > str.size())
			return str_npos;

		std::size_t max_offset = str.size() - substr.size();

		//Find the first instance of the first character of the substring after the start index
		while (true) {
			offset = str.find_first_of(substr.at(0), offset);

			if (offset == str_npos || offset > max_offset)
				return str_npos;

			bool not_match = false;
			for (size_t i = 1; i < substr.size(); i++) {
				if (str.at(offset + i) != substr.at(i)) {
					not_match = true;
					break;
				}
			}

			if (!not_match)
				return offset;

			offset++;
		}
	}

	bool containsSubstring(const std::string& str, const std::string& substr, size_t offset) {
		return findSubstring(str, substr, offset) != str_npos;
	}

	size_t findAnyChar(const std::string& str, const std::string& chars, size_t offset) {
		for (std::string::size_type i = offset; i < str.size(); i++) {
			if (chars.find(str.at(i)) != std::string::npos)
				return i;
		}
		return std::string::npos;
	}

	bool nonCaseSensitiveEquals(const std::string& str1, const std::string& str2) {
		if (str1.size() != str2.size())
			return false;

		for (std::string::size_type i = 0u; i < str1.size(); i++) {
			if (std::tolower(str1[i]) != std::tolower(str2[i]))
				return false;
		}

		return true;
	}

	std::string stripEnds(const std::string& str, char c) {
		if (str.empty())
			return std::string();

		std::string::size_type first_index = 0u;
		while (first_index < str.length() && str.at(first_index) == c)
			first_index++;

		if (first_index >= str.size())
			return std::string();

		std::string::size_type last_index = str.length() - 1u;
		while (last_index >= 0 && str.at(last_index) == c)
			last_index--;

		if (last_index < 0)
			return std::string();

		return str.substr(first_index, last_index - first_index + 1);
	}

	unsigned int splitString(const std::string& str, char delim, std::vector<std::string>& out) {
		std::string::size_type index = str.find_first_not_of(delim);
		std::string::size_type next_index = 0u;
		unsigned int count = 0u;

		while (next_index != std::string::npos) {
			next_index = str.find_first_of(delim, index);

			if (next_index > index) {
				out.push_back(str.substr(index, next_index - index));
				count++;
			}

			index = next_index + 1;
		}

		return count;
	}
}
