#pragma once
#include "Panel.h"
#include "EditorStyleHandler.h"

class Editor;

class PanelScene : public Panel
{
public:
	PanelScene(Editor* editor);
	~PanelScene();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	EditorStyleHandler styleHandler;

	// Needed modules
	Editor* editor = nullptr;
};