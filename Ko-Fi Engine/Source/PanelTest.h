#pragma once
#include "Panel.h"

class PanelTest : public Panel
{
public:

	PanelTest();
	~PanelTest();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
};