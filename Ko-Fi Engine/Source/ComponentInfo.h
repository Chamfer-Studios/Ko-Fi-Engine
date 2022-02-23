#pragma once
#include "Component.h"
using Json = nlohmann::json;

class ComponentInfo : public Component
{
public:
	ComponentInfo(GameObject* parent);
	~ComponentInfo();

	bool InspectorDraw(PanelChooser* chooser);
	Json Save() override;
	void Load(Json json) override;

private:
};