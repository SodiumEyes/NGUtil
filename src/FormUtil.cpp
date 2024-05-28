#include "SKSEUtil/FormUtil.h"

namespace SKSEUtil {

	//Keywords
	void AddFormKeywords(RE::BGSKeywordForm* form, KeywordSet* keywords_out)
	{
		if (!form)
			return;

		for (uint32_t i = 0u; i < form->numKeywords; i++) {
			if (!form->keywords[i])
				continue;
			keywords_out->insert(form->keywords[i]);
		}
	}

	bool LocationHasKeyword(RE::BGSLocation* location, RE::BGSKeyword* keyword) {
		if (!location || !keyword)
			return false;

		while (location) {
			if (location->HasKeyword(keyword))
				return true;
			location = location->parentLoc;
		}

		return false;
	}

#define PARTICLE_TYPE_INDEX static_cast<uint32_t>(RE::BGSShaderParticleGeometryData::DataID::kParticleType)

	bool WeatherIsRain(RE::TESWeather* weather) {
		return weather && weather->precipitationData
			&& weather->precipitationData->data[PARTICLE_TYPE_INDEX].i == static_cast<uint32_t>(RE::BGSShaderParticleGeometryData::ParticleType::kRain);
	}

	bool WeatherIsSnow(RE::TESWeather* weather) {
		return weather && weather->precipitationData
			&& weather->precipitationData->data[PARTICLE_TYPE_INDEX].i == static_cast<uint32_t>(RE::BGSShaderParticleGeometryData::ParticleType::kSnow);
	}

}