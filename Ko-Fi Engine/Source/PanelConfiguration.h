#pragma once
#include "Panel.h"
#include "JsonHandler.h"
class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Json json;
	JsonHandler jsonHandler;
};