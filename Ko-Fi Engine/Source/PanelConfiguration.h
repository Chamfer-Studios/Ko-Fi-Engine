#pragma once
#include "Panel.h"
class Window;
class Renderer3D;
struct EngineConfig;
class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(Window* window,Renderer3D* renderer,EngineConfig* engineConfig);
	~PanelConfiguration();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
	EngineConfig* engineConfig = nullptr;
};