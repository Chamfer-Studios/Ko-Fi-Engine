#pragma once
#include "Panel.h"

class Window;
class Renderer3D;
class Input;
class Editor;
struct EngineConfig;

class PanelConfiguration : public Panel
{
public:

	PanelConfiguration(Window* window,Renderer3D* renderer,Input* input,EngineConfig* engineConfig,Editor* editor);
	~PanelConfiguration();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:

	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	EngineConfig* engineConfig = nullptr;
	Input* input = nullptr;
	Editor* editor = nullptr;
	bool loadingIcon = false;
};