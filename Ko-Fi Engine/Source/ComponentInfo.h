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
	void Save(Json& json) const override;
	void Load(Json& json) override;
};

#endif // !__COMPONENT_INFO_H__