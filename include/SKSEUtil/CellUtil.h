#pragma once

#include <RE/Skyrim.h>

namespace SKSEUtil
{
	bool GetAdjacentLoadedCells(RE::TESObjectREFR* target_ref, std::vector<RE::TESObjectCELL*>& cells_out);
}