#ifndef __EDITOR_STYLE_HANDLER_H__
#define __EDITOR_STYLE_HANDLER_H__

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

#endif // !__EDITOR_STYLE_HANDLER_H__