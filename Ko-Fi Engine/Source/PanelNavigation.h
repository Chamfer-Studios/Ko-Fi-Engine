#pragma once

#include "Panel.h"

class Editor;

class PanelNavigation : public Panel
{
public:
	PanelNavigation(Editor* editor);
	~PanelNavigation();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	Editor* editor = nullptr;
};
