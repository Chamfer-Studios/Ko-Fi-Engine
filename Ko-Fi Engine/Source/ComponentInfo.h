#ifndef __COMPONENT_INFO_H__
#define __COMPONENT_INFO_H__

#include "Component.h"

using Json = nlohmann::json;

class ComponentInfo : public Component
{
public:
	ComponentInfo(GameObject* parent);
	~ComponentInfo();

	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;
	void Save(Json& json) const override;
	void Load(Json& json) override;

private:
	int tag;
};

#endif // !__COMPONENT_INFO_H__