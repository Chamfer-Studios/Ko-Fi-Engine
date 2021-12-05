#include "JsonHandler.h"
#include "SDL_assert.h"
#include <fstream>
#include <iomanip>
#include "Log.h"
#include "ImGuiAppLog.h"

bool JsonHandler::SaveJson(Json& json, const char* path) const
{
	bool ret = false;
	SDL_assert(path != nullptr);
	std::ofstream stream(path);
	SDL_assert(stream.is_open());
	try
	{
		stream << std::setw(4) << json << std::endl;
		ret = true;

	}
	catch (Json::parse_error& e)
	{
		CONSOLE_LOG("Error while Saving File: %c", e.what());
		appLog->AddLog("Error while Saving File: %c\n", e.what());
		ret = false;
	}
	stream.close();
	return ret;
}

bool JsonHandler::LoadJson(Json& json,const char* path) const
{
	bool ret = true;
	SDL_assert(path != nullptr);
	std::ifstream stream(path);
	SDL_assert(stream.is_open());
	try {
		json = Json::parse(stream);
		ret = true;
	}
	catch (Json::parse_error& e) {
		CONSOLE_LOG("Error while Loading File: %c", e.what());
		appLog->AddLog("Error while Loading File: %c\n", e.what());
	}	
	stream.close();

	return ret;
}

std::string JsonHandler::JsonToString(Json json)
{
	std::string ret;
	ret = json.dump(4);
	return ret;
}

Json JsonHandler::StringToJson(std::string string)
{
	Json ret;
	ret = Json::parse(string);
	return ret;
}
