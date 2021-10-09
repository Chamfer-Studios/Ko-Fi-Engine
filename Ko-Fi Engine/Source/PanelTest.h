#pragma once
#include "Panel.h"
#include "EditorStyleHandler.h"

class PanelTest : public Panel
{
public:

	PanelTest();
	~PanelTest();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	EditorStyleHandler styleHandler;

};