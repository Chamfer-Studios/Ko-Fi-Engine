#ifndef __COMPONENT_INFO_H__
#define __COMPONENT_INFO_H__

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
};

#endif // !__COMPONENT_INFO_H__