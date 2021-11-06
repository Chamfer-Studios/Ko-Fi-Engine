#pragma once
#include "Panel.h"
#include "EditorStyleHandler.h"

class Editor;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(Editor* editor);
	~PanelHierarchy();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	EditorStyleHandler styleHandler;

	// Needed modules
	Editor* editor = nullptr;
};