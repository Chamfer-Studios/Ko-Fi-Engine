#pragma once
#include "Panel.h"


class Editor;
struct EngineConfig;

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(Editor* editor, EngineConfig* engineConfig);
	~PanelConfiguration();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	EngineConfig* engineConfig = nullptr;
	Editor* editor = nullptr;
	bool modifyAttributesMenu = false;
	bool wireframe = false;
	bool loadingIcon = false;
};