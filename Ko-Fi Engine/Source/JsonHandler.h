#pragma once
#include "json.hpp"
#include "SString.h"

using Json = nlohmann::json;
class JsonHandler
{
public:
	// Constructor
	JsonHandler() {};

	bool SaveJson(Json& obj, const char* file) const;
	Json LoadJson(const char* file) const;
	std::string JsonToString(Json file);
	Json StringToJson(std::string string);
};