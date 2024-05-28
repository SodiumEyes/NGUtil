#pragma once

#include <RE/Skyrim.h>

namespace SKSEUtil
{
	typedef std::set<RE::BGSKeyword*> KeywordSet;
	typedef std::set<RE::TESForm*> FormSet;
	typedef std::map<RE::TESForm*, int> FormIntMap;

	//Keywords
	void AddFormKeywords(RE::BGSKeywordForm* form, KeywordSet* keywords_out);
	bool LocationHasKeyword(RE::BGSLocation* location, RE::BGSKeyword* keyword);

	bool WeatherIsRain(RE::TESWeather* weather);
	bool WeatherIsSnow(RE::TESWeather* weather);

}