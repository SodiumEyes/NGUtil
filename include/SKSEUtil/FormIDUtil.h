#pragma once

#include "json/json.h"
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace SKSEUtil
{
	typedef std::map<int, RE::TESFile*> IdFileMap;
	typedef std::map<std::string, RE::TESFile*> StringTESFileMap;

	void serializeFormID(RE::FormID form_id, Json::Value& json_value_out);
	bool deserializeFormID(const Json::Value& json_value, RE::FormID& form_id_out, std::string& mod_name_out);
	bool deserializeFormIDString(const std::string& str, RE::FormID& form_id_out, std::string& mod_name_out);
	RE::TESForm* tryLookupFormIdJson(const Json::Value& json_value, bool try_editor_id = false);
	RE::TESForm* tryLookupFormIdString(const std::string& str, bool try_editor_id=false);

	RE::FormID trimFileFromFormID(RE::FormID form_id, bool light_plugin=false);

	RE::TESFile* getFileByName(const std::string& name);
	RE::TESFile* getFormIdFile(RE::FormID form_id);
	std::string getFormIdFilename(RE::FormID form_id);

	void lookupFormError(RE::FormID form_id, const std::string& mod_name, const std::string& error);

	template <class FormT>
	bool LookupForm(RE::FormID form_id, const std::string& mod_name, FormT*& out, bool allow_null=false)
	{
		RE::TESForm* res = RE::TESDataHandler::GetSingleton()->LookupForm(form_id, mod_name);
		if (res) {
			out = res->As<FormT>();
			if (!out) {
				lookupFormError(form_id, mod_name, "Not Found");
				return false;
			}
			return true;
		}
		else if (!allow_null)
			lookupFormError(form_id, mod_name, "Not Found");

		return false;
	}

	template <class FormT>
	bool LookupSaveJsonForm(SKSE::SerializationInterface* serde, const Json::Value& form_id_json, FormT*& form_out) {
		if (!form_id_json.isIntegral()) {
			form_out = NULL;
			return false;
		}

		RE::FormID form_id = 0u;
		if (!serde)
			form_id = form_id_json.asUInt();
		else if (!serde->ResolveFormID(static_cast<RE::FormID>(form_id_json.asUInt()), form_id)) {
			form_out = NULL;
			return false;
		}

		RE::TESForm* form = RE::TESForm::LookupByID(form_id);
		if (form) {
			form_out = form->As<FormT>();
			return form_out != NULL;
		}
		else {
			form_out = NULL;
			return false;
		}
	}

	//FormID Cooldown Map
	struct FormIDCooldownMap {
		typedef std::map<RE::FormID, float> MapType;
		MapType map;
		float cooldown;

		FormIDCooldownMap();
		bool empty() const;
		
		bool applyCooldown(RE::FormID form_id, float time);
		bool isInCooldown(RE::FormID form_id, float time);
		std::size_t clean(float time);

		void serializeJson(SKSE::SerializationInterface* serde, Json::Value& json_value);
		bool deserializeJson(SKSE::SerializationInterface* serde, const Json::Value& json_value);
	};
	
};
