#pragma once

#include "json/json.h"
#include <RE/Skyrim.h>

namespace SKSEUtil
{
	typedef std::map<int, RE::TESFile*> IdFileMap;
	typedef std::map<std::string, RE::TESFile*> StringTESFileMap;

	void serializeFormID(RE::FormID form_id, Json::Value& json_value_out);
	bool deserializeFormID(const Json::Value& json_value, RE::FormID& form_id_out, std::string& mod_name_out);
	bool deserializeFormIDString(const std::string& str, RE::FormID& form_id_out, std::string& mod_name_out);
	RE::TESForm* tryLookupFormIdJson(const Json::Value& json_value);
	RE::TESForm* tryLookupFormIdString(const std::string& str);

	RE::FormID trimFileFromFormID(RE::FormID form_id, bool light_plugin=false);

	RE::TESFile* getFileByName(const std::string& name);
	RE::TESFile* getFormIdFile(RE::FormID form_id);
	std::string getFormIdFilename(RE::FormID form_id);
};
