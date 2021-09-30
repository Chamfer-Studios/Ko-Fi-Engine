#pragma once
#include "Panel.h"
#include "JsonHandler.h"
class Window;
class Renderer3D;
class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(Window* window,Renderer3D* renderer);
	~PanelConfiguration();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Json json;
	JsonHandler jsonHandler;
	Window* window = nullptr;
	Renderer3D* renderer = nullptr;
};