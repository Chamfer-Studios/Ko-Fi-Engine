#pragma once
#include "JsonHandler.h"

class ImGuiStyle;

class EditorStyleHandler
{
public:
	EditorStyleHandler();
	~EditorStyleHandler();
	ImGuiStyle* SetKoFiStyle();

private:
	JsonHandler jsonHandler;
	Json styleJson;
};