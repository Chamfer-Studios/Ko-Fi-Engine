#include "Scripting.h"

bool Scripting::LoadJsonFile(const char* path)
{
	for (std::map<std::string, Json>::iterator file = files.begin(); file != files.end(); ++file)
	{
		if ((*file).first == path)
		{
			KOFI_ERROR("JSON FILE ALREADY OPENED ON LUA");
			return false;
		}
	}

	JsonHandler jsonHandler;
	Json newFile;

	bool status = jsonHandler.LoadJson(newFile, path);
	if (status == false) {
		KOFI_ERROR("Fatal error on LoadJSON(),scripting.h FILE DOES NOT EXIST");
		return false;
	}
	files.insert({ path, newFile });
	CONSOLE_LOG("%s json loaded", path);

	return true;

}

std::string Scripting::GetDialogueString(const char* key, int id)
{
	const char* path = "Assets/Dialogues/dialogues.json";
	for (std::map<std::string, Json>::iterator file = files.begin(); file != files.end(); ++file)
	{
		if ((*file).first == path)
		{
			KOFI_DEBUG("File Found: %s", (*file).first);
			Json dialogues = (*file).second.at("dialogues");
			for (const auto& node : dialogues.items())
			{
				if (node.value().at("id") == id)
				{
					std::string a = node.value().at(key);
					return a;
				}
			}


		}
	}
	return "no value";
}

int Scripting::GetDialogueTargetID(const char* key, int id)
{
	const char* path = "Assets/Dialogues/dialogues.json";
	for (std::map<std::string, Json>::iterator file = files.begin(); file != files.end(); ++file)
	{
		if ((*file).first == path)
		{
			KOFI_DEBUG("File Found: %s", (*file).first);
			Json dialogues = (*file).second.at("dialogues");
			for (const auto& node : dialogues.items())
			{
				if (node.value().at("id") == id)
				{
					int targetID = node.value().at(key);
					return targetID;
				}
			}


		}
	}
}
