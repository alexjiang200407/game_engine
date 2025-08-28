#include "util/Settings.h"
#include "util/str.h"
#include <SimpleIni.h>

static constexpr auto iniFilePath = "config/settings.ini";

void
util::Settings::ReadAllSettings()
{
	logger::info("Reading all MCM settings");

	auto newSectionMap = std::make_shared<Data>();
	ReadSettings(iniFilePath, *newSectionMap);
	//newSectionMap->ReadSettings(userIniFilePath.data(), *newSectionMap);
	std::atomic_store(&GetSingleton()->data, std::move(newSectionMap));
}

util::Settings*
util::Settings::GetSingleton()
{
	static Settings singleton;
	return &singleton;
}

void
util::Settings::ReadSettings(std::string_view filePath, Data& a_data)
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(filePath.data());
	if (rc < 0)
	{
		logger::error("Ini File {} not found", filePath);
		return;
	}

	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);

	for (const auto& section : sections)
	{
		const char* sectionName = section.pItem;

		const CSimpleIniA::TKeyVal* keys = ini.GetSection(sectionName);
		if (!keys)
			continue;

		for (const auto& keyValue : *keys)
		{
			const char* key = keyValue.first.pItem;
			const char* valueAsStr = keyValue.second;

			if (!key || key[0] == 0)
				continue;

			logger::trace("Reading Setting {} {} {}", sectionName, key, valueAsStr);

			if (key[0] == 'b')
			{
				bool value;
				if (!util::parseStr(valueAsStr, value))
					continue;
				Set(a_data, sectionName, key, value);
			}
			else if (key[0] == 'i')
			{
				int value;
				if (!util::parseStr(valueAsStr, value))
					continue;
				Set(a_data, sectionName, key, value);
			}
			else if (key[0] == 'u')
			{
				unsigned int value;
				if (!util::parseStr(valueAsStr, value))
					continue;
				Set(a_data, sectionName, key, value);
			}
			else if (key[0] == 'f')
			{
				float value;
				if (!util::parseStr(valueAsStr, value))
					continue;
				Set(a_data, sectionName, key, value);
			}
			else if (key[0] == 's')
			{
				Set(a_data, sectionName, key, std::string(valueAsStr));
			}
			else
			{
				logger::warn("Invalid Setting a_key format {}", key);
			}
		}
	}
}
