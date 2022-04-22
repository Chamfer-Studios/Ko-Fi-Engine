#pragma once

#include "Component.h"

class C_SoundTriggerCreator : public Component
{
public:
	C_SoundTriggerCreator(GameObject* gameObject);

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)
	void Save(Json& json) const override;
	void Load(Json& json) override;
};