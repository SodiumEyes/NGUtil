#include "SKSEUtil/FormIDUtil.h"
#include "SKSEUtil/MathUtil.h"
#include "SKSEUtil/StringUtil.h"
#include "SKSE/SKSE.h"

using namespace RE;
using namespace SKSE;

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

	RE::TESForm* tryLookupFormIdJson(const Json::Value& json_value, bool try_editor_id) {
		RE::FormID form_id;
		std::string mod_name;
		if (SKSEUtil::deserializeFormID(json_value, form_id, mod_name))
			return RE::TESDataHandler::GetSingleton()->LookupForm(form_id, mod_name);
		else if (try_editor_id && json_value.isString())
			return RE::TESForm::LookupByEditorID(json_value.asCString());

		return NULL;
	}

	RE::TESForm* tryLookupFormIdString(const std::string& str, bool try_editor_id) {
		RE::FormID form_id;
		std::string mod_name;
		if (SKSEUtil::deserializeFormIDString(str, form_id, mod_name))
			return RE::TESDataHandler::GetSingleton()->LookupForm(form_id, mod_name);
		else if (try_editor_id)
			return RE::TESForm::LookupByEditorID(str);
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

	void lookupFormError(RE::FormID form_id, const std::string& mod_name, const std::string& error) {
		SKSE::log::warn("Form Lookup Error: {}:{} :: {}", mod_name, SKSEUtil::hexToString(form_id), error);
	}

	//FormID Cooldown Map
	FormIDCooldownMap::FormIDCooldownMap()
		: cooldown(1.0) {}

	bool FormIDCooldownMap::empty() const { return map.empty(); }
	
	bool FormIDCooldownMap::applyCooldown(RE::FormID form_id, float time) {
		if (cooldown == 0.0f)
			return true;

		MapType::iterator it = map.find(form_id);
		if (it != map.end()) {
			bool was_in_cooldown = cooldown < 0.0f || (time - it->second) < cooldown;
			it->second = time;
			return !was_in_cooldown;
		}
		map[form_id] = time;
		return true;
	}

	bool FormIDCooldownMap::isInCooldown(RE::FormID form_id, float time) {
		MapType::iterator it = map.find(form_id);
		if (it != map.end()) {
			if (cooldown < 0.0f || (time - it->second) < cooldown)
				return true;
			else {
				//Cooldown elapsed so remove the iterator
				map.erase(it);
				return false;
			}
		}
		return false;
	}

	std::size_t FormIDCooldownMap::clean(float time) {
		if (cooldown < 0.0f)
			return 0u;

		if (cooldown == 0.0f) {
			std::size_t prev_size = map.size();
			map.clear();
			return prev_size;
		}

		std::vector<RE::FormID> to_delete;
		for (MapType::iterator it = map.begin(); it != map.end(); ++it) {
			if ((time - it->second) >= cooldown)
				to_delete.push_back(it->first);
		}

		for (std::size_t i = 0u; i < to_delete.size(); i++)
			map.erase(to_delete[i]);

		return to_delete.size();
	}
	
	void FormIDCooldownMap::serializeJson(SKSE::SerializationInterface*, Json::Value& json_value) {
		for (MapType::iterator it = map.begin(); it != map.end(); ++it)
			json_value[SKSEUtil::hexToString(static_cast<unsigned int>(it->first))] = it->second;
	}

	bool FormIDCooldownMap::deserializeJson(SKSE::SerializationInterface* serde, const Json::Value& json_value) {
		map.clear();
		if (!json_value.isObject())
			return false;

		for (Json::ValueConstIterator it = json_value.begin(); it != json_value.end(); ++it) {
			RE::FormID form_id = SKSEUtil::stringToHex(it.key().asCString());
			if (!serde || serde->ResolveFormID(form_id, form_id))
				map[form_id] = it->asFloat();
		}
		return true;
	}
}