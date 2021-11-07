#pragma once
#include "Panel.h"


class Editor;
struct EngineConfig;

class PanelConfiguration : public Panel
{
public:

	PanelConfiguration(EngineConfig* engineConfig,Editor* editor);
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