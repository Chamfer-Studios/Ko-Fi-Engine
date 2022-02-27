#pragma once
#include "json.hpp"
#include <string>

using Json = nlohmann::json;

class JsonHandler
{
public:
	// Constructor
	JsonHandler() {};

	bool SaveJson(Json& json, const char* path) const;
	bool LoadJson(Json& json, const char* path) const;
	std::string JsonToString(Json json);
	Json StringToJson(std::string string);
};