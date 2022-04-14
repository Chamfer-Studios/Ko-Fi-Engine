#pragma once

#include "Panel.h"

#include "MathGeoLib/Math/float3.h"

class M_Editor;

class PanelNavigation : public Panel
{
public:
	PanelNavigation(M_Editor* editor);
	~PanelNavigation();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	M_Editor* editor = nullptr;

	float3 origin = { 0, 0, 0 };
	float3 destination = { 0, 0, 0 };
};
