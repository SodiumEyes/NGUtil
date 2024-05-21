#include "SKSEUtil/CellUtil.h"

namespace SKSEUtil {

	bool GetAdjacentLoadedCells(RE::TESObjectREFR* target_ref, std::vector<RE::TESObjectCELL*>& cells_out) {
		if (!target_ref)
			return false;

		RE::TESObjectCELL* center_cell = target_ref->GetParentCell();
		if (!center_cell)
			return false;

		cells_out.clear();

		if (center_cell->IsInteriorCell()) {
			cells_out.push_back(center_cell);
			return true;
		}
		
		//Scan loaded exterior cells
		cells_out.reserve(9u);
		cells_out.push_back(center_cell);

		RE::TES* tes = RE::TES::GetSingleton();
		uint32_t num_cells = tes->gridCells->length * tes->gridCells->length;
		for (uint32_t i = 0u; i < num_cells; i++) {
			if (tes->gridCells->cells[i] == center_cell)
				continue;

			RE::TESObjectCELL::CellData& cell_data = tes->gridCells->cells[i]->GetRuntimeData().cellData;
			int x_offset = cell_data.exterior->cellX - cell_data.exterior->cellX;
			int y_offset = cell_data.exterior->cellY - cell_data.exterior->cellY;
			if (x_offset >= -1 && x_offset <= 1 && y_offset >= -1 && y_offset <= 1)
				cells_out.push_back(tes->gridCells->cells[i]);
		}

		return true;
	}

}