#include "SKSEUtil/EnumUtil.h"
#include "SKSEUtil/StringUtil.h"

namespace SKSEUtil {

	bool GetCastingSourceFromName(const std::string& name, RE::MagicSystem::CastingSource& out) {
		if (SKSEUtil::nonCaseSensitiveEquals(name, "left")) {
			out = RE::MagicSystem::CastingSource::kLeftHand;
			return true;
		}
		else if (SKSEUtil::nonCaseSensitiveEquals(name, "right")) {
			out = RE::MagicSystem::CastingSource::kRightHand;
			return true;
		}
		else if (SKSEUtil::nonCaseSensitiveEquals(name, "voice")) {
			out = RE::MagicSystem::CastingSource::kInstant;
			return true;
		}
		else if (SKSEUtil::nonCaseSensitiveEquals(name, "instant")) {
			out = RE::MagicSystem::CastingSource::kInstant;
			return true;
		}
		else if (SKSEUtil::nonCaseSensitiveEquals(name, "other")) {
			out = RE::MagicSystem::CastingSource::kOther;
			return true;
		}
		return false;
	}

	std::string CastingTypeName(RE::MagicSystem::CastingType type) {
		switch (type) {
		case RE::MagicSystem::CastingType::kConcentration:
			return "Concentration";
		case RE::MagicSystem::CastingType::kConstantEffect:
			return "Constant Effect";
		case RE::MagicSystem::CastingType::kFireAndForget:
			return "Fire And Forget";
		case RE::MagicSystem::CastingType::kScroll:
			return "Scroll";
		default:
			return std::string();
		}
	}

	std::string MagicDeliveryTypeName(RE::MagicSystem::Delivery type) {
		switch (type) {
		case RE::MagicSystem::Delivery::kAimed:
			return "Aimed";
		case RE::MagicSystem::Delivery::kSelf:
			return "Self";
		case RE::MagicSystem::Delivery::kTargetActor:
			return "Target Actor";
		case RE::MagicSystem::Delivery::kTargetLocation:
			return "Target Location";
		case RE::MagicSystem::Delivery::kTouch:
			return "Contact";
		default:
			return std::string();
		}
	}

	std::string EmotionTypeName(RE::EmotionType type) {
		switch (type) {
		case RE::EmotionType::kAnger:
			return "Anger";
		case RE::EmotionType::kDisgust:
			return "Disgust";
		case RE::EmotionType::kFear:
			return "Fear";
		case RE::EmotionType::kHappy:
			return "Happy";
		case RE::EmotionType::kNeutral:
			return "Neutral";
		case RE::EmotionType::kPuzzled:
			return "Puzzled";
		case RE::EmotionType::kSad:
			return "Sad";
		case RE::EmotionType::kSurprise:
			return "Surprise";
		default:
			return std::string();
		}
	}

	std::string KnockedStateName(RE::KNOCK_STATE_ENUM state) {
		switch (state) {
		case RE::KNOCK_STATE_ENUM::kNormal:
			return "Normal";
		case RE::KNOCK_STATE_ENUM::kQueued:
			return "Queued";
		case RE::KNOCK_STATE_ENUM::kGetUp:
			return "GetUp";
		case RE::KNOCK_STATE_ENUM::kExplode:
		case RE::KNOCK_STATE_ENUM::kExplodeLeadIn:
			return "Explode";
		case RE::KNOCK_STATE_ENUM::kOut:
		case RE::KNOCK_STATE_ENUM::kOutLeadIn:
			return "Out";
		case RE::KNOCK_STATE_ENUM::kDown:
			return "Down";
		default:
			return std::string();
		}
	}

	int32_t RelationshipLevelToRank(RE::BGSRelationship::RELATIONSHIP_LEVEL level) {
		switch (level) {
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kArchnemesis:
			return -4;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kEnemy:
			return -3;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kFoe:
			return -2;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kRival:
			return -1;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kAcquaintance:
			return 0;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kFriend:
			return 1;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kConfidant:
			return 2;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kAlly:
			return 3;
		case RE::BGSRelationship::RELATIONSHIP_LEVEL::kLover:
			return 4;
		default:
			return 0;
		}
	}
}