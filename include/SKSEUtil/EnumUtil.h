#pragma once

#include <RE/Skyrim.h>

namespace SKSEUtil
{
	bool GetCastingSourceFromName(const std::string& name, RE::MagicSystem::CastingSource& out);
	std::string CastingTypeName(RE::MagicSystem::CastingType type);
	std::string MagicDeliveryTypeName(RE::MagicSystem::Delivery type);
	std::string EmotionTypeName(RE::EmotionType type);
	int32_t RelationshipLevelToRank(RE::BGSRelationship::RELATIONSHIP_LEVEL level);
}