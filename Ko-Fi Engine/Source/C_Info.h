#ifndef __C_INFO_H__
#define __C_INFO_H__

#include "Component.h"

using Json = nlohmann::json;

class C_Info : public Component
{
public:
	C_Info(GameObject* parent);
	virtual ~C_Info();

	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;
	void Save(Json& json) const override;
	void Load(Json& json) override;

private:
	int tag = 0;
	bool showPopup = false;
};

#endif // !__C_INFO_H__