#include "SKSEUtil/FormIDUtil.h"
#include "SKSEUtil/MathUtil.h"
#include "SKSEUtil/StringUtil.h"

using namespace RE;

namespace SKSEUtil {

	IdFileMap sFormIdFileMap;
	StringTESFileMap sFileNameMap;

	void initFileMaps() {
		if (sFormIdFileMap.empty()) {
			//Init formID file map
			TESDataHandler* data_handler = TESDataHandler::GetSingleton();
			for (RE::BSSimpleList<TESFile*>::iterator it = data_handler->files.begin(); it != data_handler->files.end(); it++) {
				TESFile* f = *it;
				std::string filename = f->fileName;
				if (!f->IsLight())
					sFormIdFileMap[f->compileIndex] = f;
				sFileNameMap[toLowercaseString(filename)] = f;
			}
		}
	}

	void serializeFormID(RE::FormID form_id, Json::Value& json_value_out) {
		RE::TESFile* file = getFormIdFile(form_id);
		if (file) {
			std::string result = file->fileName;
			result.push_back(':');
			result.append(hexToString(trimFileFromFormID(form_id, file->IsLight())));
			json_value_out = result;
		}
		else
			json_value_out = "0";
	}

	bool deserializeFormID(const Json::Value& json_value, RE::FormID& form_id_out, std::string& mod_name_out)
	{
		if (json_value.isString()) {
			return deserializeFormIDString(json_value.asCString(), form_id_out, mod_name_out);
		}
		else if (json_value.isNumeric()) {
			if (mod_name_out.empty()) {
				form_id_out = static_cast<RE::FormID>(json_value.asUInt());
				return true;
			}
			else
				return false;
		}

		return false;
	}

	bool deserializeFormIDString(const std::string& str, RE::FormID& form_id_out, std::string& mod_name_out) {
		std::string::size_type delim_index = str.find_first_of(':');
		if (delim_index != std::string::npos) {
			mod_name_out = str.substr(0u, delim_index);
			std::string::size_type next_delim_index = str.find_first_of(':', delim_index + 1);
			std::string form_id_str;
			if (next_delim_index != std::string::npos)
				form_id_str = str.substr(delim_index + 1, next_delim_index - (delim_index + 1)); //Ignore anything after the second :
			else
				form_id_str = str.substr(delim_index + 1);
			RE::TESFile* file = getFileByName(mod_name_out);
			if (file && mod_name_out != file->fileName) //Correct casing of the mod filename
				mod_name_out = file->fileName;
			form_id_out = trimFileFromFormID(stringToHex(form_id_str), file && file->IsLight());
			return true;
		}
		else if (!mod_name_out.empty()) {
			form_id_out = trimFileFromFormID(stringToHex(str));
			return true;
		}
		return false;
	}

	RE::TESForm* tryLookupFormIdJson(const Json::Value& json_value) {
		RE::FormID form_id;
		std::string mod_name;
		if (SKSEUtil::deserializeFormID(json_value, form_id, mod_name))
			return RE::TESDataHandler::GetSingleton()->LookupForm(form_id, mod_name);
		return NULL;
	}

	RE::TESForm* tryLookupFormIdString(const std::string& str) {
		RE::FormID form_id;
		std::string mod_name;
		if (SKSEUtil::deserializeFormIDString(str, form_id, mod_name))
			return RE::TESDataHandler::GetSingleton()->LookupForm(form_id, mod_name);
		return NULL;
	}

	RE::FormID trimFileFromFormID(RE::FormID form_id, bool light_plugin) {
		if (!light_plugin)
			return form_id & 0x00FFFFFF;
		else
			return form_id & 0x00000FFF;
	}

	RE::TESFile* getFileByName(const std::string& name) {
		initFileMaps();
		StringTESFileMap::iterator it = sFileNameMap.find(toLowercaseString(name));
		if (it != sFileNameMap.end())
			return it->second;
		return NULL;
	}

	RE::TESFile* getFormIdFile(RE::FormID form_id) {
		initFileMaps();

		//Try using the id header byte to determine the file
		int id_header = (form_id & 0xFF000000);
		int file_index = id_header >> 24;
		IdFileMap::iterator offset_file = sFormIdFileMap.find(file_index);

		if (offset_file != sFormIdFileMap.end())
			return offset_file->second;

		//Try using the stored file handle of the form itself (not always accurate but needed for esl)
		RE::TESForm* form = RE::TESForm::LookupByID(form_id);
		if (form)
			return form->GetFile();

		return NULL;
	}

	std::string getFormIdFilename(RE::FormID form_id) {
		RE::TESFile* file = getFormIdFile(form_id);
		if (file)
			return file->fileName;
		return std::string();
	}

}