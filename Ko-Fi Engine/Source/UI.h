#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "GameObject.h"

#include "glew.h"
#include <vector>

class ComponentCamera;

class UI : public Module
{
public:
	UI(KoFiEngine* engine);
	~UI();

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	float2 GetUINormalizedMousePosition();

	void OnGui() override;
	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;
public:
	GLint uiCameraViewport[4] = { 0, 0, 0, 0 };

private:
	KoFiEngine* engine = nullptr;
};