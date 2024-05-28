#include "SKSEUtil/ActorUtil.h"

using namespace SKSE;
using namespace RE;

namespace SKSEUtil
{
	RE::TESObjectARMO* GetWornInSlot(RE::Actor* thisActor, int32_t slot) {
		RE::ExtraContainerChanges* pContainerChanges = static_cast<RE::ExtraContainerChanges*>(thisActor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
		if (pContainerChanges) {
			return pContainerChanges->changes->GetArmorInSlot(slot);
		}
		return NULL;
	}

	//Action Queue
	void addInventoryDelta(RE::TESForm* form, int delta, InventoryDeltaMap* delta_map, ItemCountMap* item_counts) {
		if (delta == 0 || !form)
			return;

		if (item_counts) {
			ItemCountMap::iterator count_it = item_counts->find(form);
			int32_t count = 0;
			if (count_it != item_counts->end())
				count = count_it->second;

			int32_t new_count = count;
			new_count += delta;

			if (new_count < 0) {
				new_count = 0;
				delta = -count;
				if (delta == 0)
					return;
			}

			(*item_counts)[form] = new_count;
		}

		if (delta_map) {
			InventoryDeltaMap::iterator it = delta_map->find(form);
			if (it == delta_map->end())
				(*delta_map)[form] = delta;
			else
				it->second += delta;
		}
	}

	void addEquipAction(RE::TESForm* form, bool equip, EquipActionMap* action_map, FormSet* worn_forms) {
		if (!form)
			return;

		if (action_map)
			(*action_map)[form] = equip;

		if (worn_forms) {
			if (equip)
				worn_forms->insert(form);
			else
				worn_forms->erase(form);
		}
	}

	//Items

	InventoryFormState::InventoryFormState() :
		object(NULL),
		worn(false),
		countDelta(0)
	{}
	InventoryFormState::InventoryFormState(const RE::InventoryEntryData& entry) :
		object(entry.object),
		worn(entry.IsWorn()),
		countDelta(entry.countDelta)
	{
	}

	int32_t ApplyItemCount(ItemCountMap& item_counts, RE::TESForm* item_form, int32_t count)
	{
		if (!item_form || count == 0)
			return 0;

		ItemCountMap::iterator it = item_counts.find(item_form);
		if (it == item_counts.end())
			it = item_counts.insert(std::pair<TESForm*, uint32_t>(item_form, count)).first;
		else
			it->second += count;

		return count;
	}

	int32_t GetItemCounts(RE::TESObjectREFR* cont, ItemCountMap& out)
	{
		if (!cont)
			return 0;

		std::int32_t count = 0;
		RE::TESContainer* ref_cont = cont->GetContainer();
		for (std::uint32_t i = 0u; i < ref_cont->numContainerObjects; i++)
			count += ApplyItemCount(out, ref_cont->containerObjects[i]->obj->As<TESForm>(), ref_cont->containerObjects[i]->count);

		RE::ExtraContainerChanges* pContainerChanges = static_cast<RE::ExtraContainerChanges*>(cont->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
		if (pContainerChanges) {
			for (BSSimpleList<InventoryEntryData*>::iterator it = pContainerChanges->changes->entryList->begin(); it != pContainerChanges->changes->entryList->end(); ++it)
			{
				InventoryEntryData* e = *it;
				if (!e)
					continue;
				count += ApplyItemCount(out, e->object->As<TESForm>(), e->countDelta);
			}
		}

		return count;
	}

	int32_t GetItemCount(ItemCountMap& item_counts, RE::TESForm* form) {
		if (!form)
			return 0;

		ItemCountMap::iterator it = item_counts.find(form);
		if (it == item_counts.end())
			return 0;
		else
			return it->second;
	}

	bool AddItem(RE::TESObjectREFR* cont, RE::TESForm* form, int32_t count, ItemCountMap* item_counts)
	{
		if (!cont || !form || count <= 0)
			return false;

		RE::ExtraContainerChanges* pContainerChanges = static_cast<RE::ExtraContainerChanges*>(cont->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
		TESBoundObject* bound = form->As<TESBoundObject>();
		if (!bound || !pContainerChanges)
			return false;

		BSSimpleList<InventoryEntryData*>::const_iterator last_it = pContainerChanges->changes->entryList->cend();
		for (BSSimpleList<InventoryEntryData*>::const_iterator it = pContainerChanges->changes->entryList->cbegin(); it != pContainerChanges->changes->entryList->cend(); ++it) {
			last_it = it;
			if ((*it)->object == bound) {
				(*it)->countDelta += count;
				pContainerChanges->changes->changed = true;
				return true;
			}
		}

		if (last_it == pContainerChanges->changes->entryList->cend())
			pContainerChanges->changes->AddEntryData(new InventoryEntryData(bound, count));
		else
			pContainerChanges->changes->entryList->insert_after(last_it, new InventoryEntryData(bound, count));

		if (item_counts)
			(*item_counts)[form] = GetItemCount(*item_counts, form) + count;

		pContainerChanges->changes->changed = true;
		return true;
	}

	void RemoveAllOfItem(RE::TESObjectREFR* cont, RE::TESForm* form, ItemCountMap& item_counts)
	{
		RemoveItem(cont, form, GetItemCount(item_counts, form), &item_counts);
	}

	void RemoveItem(RE::TESObjectREFR* cont, RE::TESForm* form, int32_t count, ItemCountMap* item_counts)
	{
		if (count <= 0)
			return;

		cont->RemoveItem(form->As<TESBoundObject>(), count, RE::ITEM_REMOVE_REASON::kRemove, NULL, NULL);
		if (item_counts)
			(*item_counts)[form] = std::max(GetItemCount(*item_counts, form) - count, 0);
	}

	//Equips

	int32_t GetWornForms(RE::Actor* a, FormSet* forms_out, KeywordSet* keywords_out)
	{
		if (!a)
			return 0;

		int32_t count = 0;
		RE::ExtraContainerChanges* pContainerChanges = static_cast<RE::ExtraContainerChanges*>(a->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
		if (pContainerChanges) {
			for (BSSimpleList<InventoryEntryData*>::iterator it = pContainerChanges->changes->entryList->begin(); it != pContainerChanges->changes->entryList->end(); ++it) {
				InventoryEntryData* e = *it;
				if (e && e->IsWorn()) {
					TESForm* form = e->object->As<TESForm>();
					if (form) {
						if (forms_out)
							forms_out->insert(form);

						if (keywords_out)
							AddFormKeywords(form->As<BGSKeywordForm>(), keywords_out);
						count++;
					}
				}
			}
		}

		return count;
	}

	void GetWornKeywords(FormSet& worn_forms, KeywordSet* keywords_out) {
		for (FormSet::iterator it = worn_forms.begin(); it != worn_forms.end(); ++it)
			AddFormKeywords((*it)->As<BGSKeywordForm>(), keywords_out);
	}

	//Magic
	RE::MagicItem* GetEquippedSpell(RE::Actor* a, RE::MagicSystem::CastingSource source) {
		if (!a)
			return NULL;

		if (source == RE::MagicSystem::CastingSource::kInstant) {
			if (a->GetActorRuntimeData().selectedPower)
				return a->GetActorRuntimeData().selectedPower->As<RE::MagicItem>();
			else
				return NULL;
		}
		else
			return a->GetActorRuntimeData().selectedSpells[static_cast<unsigned int>(source)];
	}

	RE::TESShout* GetEquippedShout(RE::Actor* a) {
		if (!a || !a->GetActorRuntimeData().selectedPower)
			return NULL;

		return a->GetActorRuntimeData().selectedPower->As<RE::TESShout>();
	}

	//Factions

	int8_t GetFactionRank(RE::Actor* a, TESFaction* faction) {
		auto factionChanges = a->extraList.GetByType<ExtraFactionChanges>();
		if (factionChanges) {
			for (auto& change : factionChanges->factionChanges) {
				if (change.faction == faction) {
					return change.rank;
				}
			}
		}

		for (unsigned int i = 0u; i < a->GetActorBase()->factions.size(); i++) {
			if (a->GetActorBase()->factions[i].faction == faction)
				return a->GetActorBase()->factions[i].rank;
		}

		return -2;
	}

	int8_t GetFactionRank(FactionRankMap& faction_ranks, RE::TESFaction* faction) {
		FactionRankMap::iterator it = faction_ranks.find(faction);
		if (it != faction_ranks.end())
			return it->second;
		return -2;
	}

	bool IsInFaction(FactionRankMap& faction_ranks, RE::TESFaction* faction) {
		return GetFactionRank(faction_ranks, faction) >= 0;
	}

	bool SetFactionRank(RE::Actor* a, TESFaction* faction, int8_t rank) {
		//Apply via existing ExtraFactionChanges
		ExtraFactionChanges* factionChanges = a->extraList.GetByType<ExtraFactionChanges>();
		if (factionChanges) {
			for (auto& change : factionChanges->factionChanges) {
				if (change.faction == faction) {
					if (change.rank != rank) {
						change.rank = rank;
						return true;
					}
					else
						return false;
				}
			}

			FACTION_RANK new_rank;
			new_rank.faction = faction;
			new_rank.rank = rank;
			new_rank.pad09 = 0;
			new_rank.pad0A = 0;
			new_rank.pad0C = 0;
			factionChanges->factionChanges.push_back(new_rank);
			return true;
		}

		//Set via BaseActor faction list
		for (unsigned int i = 0u; i < a->GetActorBase()->factions.size(); i++) {
			if (a->GetActorBase()->factions[i].faction == faction) {
				if (a->GetActorBase()->factions[i].rank == rank)
					return false;
				else if (!factionChanges) {
					a->GetActorBase()->factions[i].rank = rank;
					return true;
				}
			}
		}

		//Add new rank entry
		FACTION_RANK new_rank;
		new_rank.faction = faction;
		new_rank.rank = rank;
		new_rank.pad09 = 0;
		new_rank.pad0A = 0;
		new_rank.pad0C = 0;
		a->GetActorBase()->factions.push_back(new_rank);
		return true;
	}

	bool RemoveFromFaction(RE::Actor* a, RE::TESFaction* faction) {
		ExtraFactionChanges* factionChanges = a->extraList.GetByType<ExtraFactionChanges>();
		if (factionChanges) {
			for (auto& change : factionChanges->factionChanges) {
				if (change.faction == faction) {
					factionChanges->factionChanges.erase(&change);
					return true;
				}
			}
		}

		for (unsigned int i = 0u; i < a->GetActorBase()->factions.size(); i++) {
			if (a->GetActorBase()->factions[i].faction == faction) {
				a->GetActorBase()->factions.erase(&a->GetActorBase()->factions[i]);
				return true;
			}
		}
		return false;
	}

	void GetFactionRanks(RE::Actor* a, FactionRankMap* faction_ranks_out) {
		for (unsigned int i = 0u; i < a->GetActorBase()->factions.size(); i++) {
			if (a->GetActorBase()->factions[i].faction)
				(*faction_ranks_out)[a->GetActorBase()->factions[i].faction] = a->GetActorBase()->factions[i].rank;
		}

		auto factionChanges = a->extraList.GetByType<ExtraFactionChanges>();
		if (factionChanges) {
			for (auto& change : factionChanges->factionChanges) {
				if (change.faction)
					(*faction_ranks_out)[change.faction] = change.rank;
			}
		}
	}

	//Relationships
	RE::BGSRelationship::RELATIONSHIP_LEVEL GetRelationshipLevel(RE::Actor* a, RE::Actor* target) {
		static REL::Relocation<int32_t(RE::TESNPC*, RE::TESNPC*)> func{ REL::VariantID(23624, 24076, 0x355790) };
		return static_cast<RE::BGSRelationship::RELATIONSHIP_LEVEL>(func(a->GetActorBase(), target->GetActorBase()));
	}

	//Skeleton
	RE::BSFixedString* GetActorSkeleton(RE::Actor* a) {
		if (!a)
			return NULL;

		int gender_index = int(a->GetActorBase()->GetSex());
		if (gender_index != 0 && gender_index != 1)
			gender_index = 0;
		return &a->GetRace()->skeletonModels[gender_index].model;
	}

	//Actor Values
	float GetActorValueFraction(RE::Actor* a, RE::ActorValue av, bool relative_to_base) {
		if (!a || av == RE::ActorValue::kNone)
			return 0.0f;

		float base;
		if (relative_to_base)
			base = a->AsActorValueOwner()->GetBaseActorValue(av);
		else
			base = a->AsActorValueOwner()->GetPermanentActorValue(av);

		if (base == 0.0f)
			return 0.0f;
		return a->AsActorValueOwner()->GetActorValue(av) / base;
	}

	//
	RE::Actor* GetKiller(RE::Actor* a) {
		if (a && a->GetActorRuntimeData().myKiller)
			return a->GetActorRuntimeData().myKiller.get().get();
		return NULL;
	}

	bool HasLineOfSight(RE::Actor* a, RE::TESObjectREFR* target, bool* a_arg2) {
		if (a_arg2)
			return a->HasLineOfSight(target, *a_arg2);
		else {
			bool temp;
			return a->HasLineOfSight(target, temp);
		}
	}

	bool IsInDialogue(RE::Actor* a) {
		if (!a)
			return false;
		static REL::Relocation<bool(RE::Actor*)> func{ REL::VariantID(36727, 37739, 0x604230) };
		return func(a);
	}

	RE::ACTOR_COMBAT_STATE GetCombatState(RE::Actor* a) {
		if (!a)
			return RE::ACTOR_COMBAT_STATE::kNone;
		static REL::Relocation<RE::ACTOR_COMBAT_STATE(RE::Actor*)> func{ REL::VariantID(37603, 38556, 0x62DD00) };
		return func(a);
	}
}
