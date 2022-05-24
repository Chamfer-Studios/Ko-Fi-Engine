#pragma once

#include "Component.h"

class KoFiEngine;

class C_SSAO : public Component
{
public:
	C_SSAO(GameObject* parent);

	bool InspectorDraw(PanelChooser* chooser) override;

	unsigned int ssaoFBO = 0;
	unsigned int ssaoColorBuffer = 0;
private:
};