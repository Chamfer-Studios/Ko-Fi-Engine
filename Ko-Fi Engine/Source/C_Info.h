#ifndef __COMPONENT_INFO_H__
#define __COMPONENT_INFO_H__

#include "Component.h"

using Json = nlohmann::json;

class C_Info : public Component
{
public:
	C_Info(GameObject* parent);
	~C_Info();

	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;
	void Save(Json& json) const override;
	void Load(Json& json) override;

private:
	int tag;
};

#endif // !__COMPONENT_INFO_H__