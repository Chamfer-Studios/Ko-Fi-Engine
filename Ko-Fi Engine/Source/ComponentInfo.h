#pragma once
#include "Component.h"
class ComponentInfo : public Component
{
public:
	ComponentInfo(GameObject* parent);
	~ComponentInfo();

	bool InspectorDraw(PanelChooser* chooser);

	void SetPath(const char* path);
	const char* GetPath();

private:
	const char* path = "";
};