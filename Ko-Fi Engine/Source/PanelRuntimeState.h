#pragma once
#include "Panel.h"

class Editor;
class KoFiEngine;

class PanelRuntimeState : public Panel
{
public:
	PanelRuntimeState(Editor* editor, KoFiEngine* engine);
	~PanelRuntimeState();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void DrawRuntimePanel();

private:
	Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};
