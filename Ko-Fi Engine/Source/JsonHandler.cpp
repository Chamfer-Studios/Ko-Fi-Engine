#include "JsonHandler.h"
#include "SDL_assert.h"
#include <fstream>
#include <iomanip>
#include "Log.h"

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
		LOG("Error while Saving File: %c", e.what());
		ret = false;
	}
	stream.close();
	return ret;
}

Json JsonHandler::LoadJson(const char* path) const
{
	SDL_assert(path != nullptr);
	Json json;
	std::ifstream stream(path);
	SDL_assert(stream.is_open());
	try {
		json = Json::parse(stream);
	}
	catch (Json::parse_error& e) {
		LOG("Error while Loading File: %c", e.what());
	}	
	stream.close();

	return json;
}

std::string JsonHandler::JsonToString(Json file)
{
	std::string ret;
	ret = file.dump(4);
	return ret;
}

Json JsonHandler::StringToJson(std::string string)
{
	Json ret;
	ret = Json::parse(string);
	return ret;
}
