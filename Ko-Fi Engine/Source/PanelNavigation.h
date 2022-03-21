#pragma once

#include "Panel.h"

#include "MathGeoLib/Math/float3.h"

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

	float3 origin = { 0, 0, 0 };
	float3 destination = { 0, 0, 0 };
};
