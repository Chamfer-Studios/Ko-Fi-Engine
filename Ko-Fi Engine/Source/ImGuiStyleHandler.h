#pragma once
#include "JsonHandler.h"
class ImGuiStyle;
class ImGuiStyleHandler {
public:
	ImGuiStyleHandler();
	~ImGuiStyleHandler();
	ImGuiStyle* SetKoFiStyle();
private:
	JsonHandler jsonHandler;
	Json styleJson;
};