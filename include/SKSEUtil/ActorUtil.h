#pragma once

#include <RE/Skyrim.h>

namespace SKSEUtil
{
	RE::TESObjectARMO* GetWornInSlot(RE::Actor* thisActor, int32_t slot);
	const RE::FormID PlayerBaseFormID = 0x7;
	const RE::FormID PlayerRefFormID = 0x14;

	//Inventory State

	struct InventoryFormState {
		RE::TESBoundObject* object;
		bool worn;
		int32_t countDelta;

		InventoryFormState();
		InventoryFormState(const RE::InventoryEntryData& entry);
	};

	typedef std::map<RE::TESForm*, int32_t> ItemCountMap;
	typedef std::set<RE::TESForm*> FormSet;
	typedef std::set<RE::BGSKeyword*> KeywordSet;

	//Action Queue
	typedef std::map<RE::TESForm*, int> InventoryDeltaMap;
	typedef std::map<RE::TESForm*, int> FormIntMap;
	typedef std::map<RE::TESForm*, bool> EquipActionMap;

	void addInventoryDelta(RE::TESForm* form, int delta, InventoryDeltaMap* delta_map = NULL, ItemCountMap* item_counts = NULL);
	void addEquipAction(RE::TESForm* form, bool equip, EquipActionMap* action_map = NULL, FormSet* worn_forms = NULL);

	//Items
	int32_t ApplyItemCount(ItemCountMap& item_counts, RE::TESForm* item_form, int32_t count);
	int32_t GetItemCounts(RE::TESObjectREFR* cont, ItemCountMap& out);
	int32_t GetItemCount(ItemCountMap& item_counts, RE::TESForm* form);
	bool AddItem(RE::TESObjectREFR* cont, RE::TESForm* form, int32_t count, ItemCountMap* item_counts = NULL);
	void RemoveAllOfItem(RE::TESObjectREFR* cont, RE::TESForm* form, ItemCountMap& item_counts);
	void RemoveItem(RE::TESObjectREFR* cont, RE::TESForm* form, int32_t count, ItemCountMap* item_counts = NULL);

	//Equips
	int32_t GetWornForms(RE::Actor* a, FormSet* forms_out, KeywordSet* keywords_out=NULL);
	void GetWornKeywords(FormSet& worn_forms, KeywordSet* keywords_out);

	//Keywords
	void AddFormKeywords(RE::BGSKeywordForm* form, KeywordSet* keywords_out);
	
	//Factions
	typedef std::map<RE::TESFaction*, int8_t> FactionRankMap;

	int8_t GetFactionRank(RE::Actor* a, RE::TESFaction* faction);
	int8_t GetFactionRank(FactionRankMap& faction_ranks, RE::TESFaction* faction);
	bool IsInFaction(FactionRankMap& faction_ranks, RE::TESFaction* faction);
	bool SetFactionRank(RE::Actor* a, RE::TESFaction* faction, int8_t rank);
	bool RemoveFromFaction(RE::Actor* a, RE::TESFaction* faction);
	void GetFactionRanks(RE::Actor* a, FactionRankMap* faction_ranks_out);

	//Relationships
	RE::BGSRelationship::RELATIONSHIP_LEVEL GetRelationshipRank(RE::Actor* a, RE::Actor* target);
	int GetRelationshipRankInt(RE::BGSRelationship::RELATIONSHIP_LEVEL level);

	//Skeleton
	RE::BSFixedString* GetActorSkeleton(RE::Actor* a);

	//
	bool HasLineOfSight(RE::Actor* a, RE::TESObjectREFR* target, bool* a_arg2 = NULL);
	
}
