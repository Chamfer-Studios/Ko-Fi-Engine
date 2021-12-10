#include "ComponentCamera.h"
#include "GameObject.h"
#include "imgui_stdlib.h"
#include "PanelChooser.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	type = ComponentType::CAMERA;
}

ComponentCamera::~ComponentCamera()
{
}

bool ComponentCamera::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("Name:");
	}
	return ret;
}