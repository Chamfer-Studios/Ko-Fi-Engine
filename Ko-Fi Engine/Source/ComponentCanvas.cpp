#include "ComponentCanvas.h"
#include "Engine.h"
#include "Editor.h"
#include "UI.h"
#include "Window.h"
#include "GameObject.h"

#include "glew.h"

ComponentCanvas::ComponentCanvas(GameObject* parent) : ComponentTransform2D(parent)
{
	type = ComponentType::CANVAS;
	logicalSize = { 1920, 1080 };
}

void ComponentCanvas::Save(Json& json) const
{
	json["type"] = "canvas";

	json["position"] = {
		GetPosition().x,
		GetPosition().y,
	};
	json["rotation"] = {
		GetRotation().x,
		GetRotation().y,
		GetRotation().z,
	};
	json["size"] = {
		GetSize().x,
		GetSize().y
	};
	json["pivot"] = {
		GetPivot().x,
		GetPivot().y
	};
	json["logicalSize"] = {
		logicalSize.x,
		logicalSize.y
	};
	json["anchor"] = (int)GetAnchor();
}

void ComponentCanvas::Load(Json& json)
{
	std::vector<float> values = json["position"].get<std::vector<float>>();
	float2 position;
	position.x = values[0];
	position.y = values[1];

	SetPosition(position);

	values = json["rotation"].get<std::vector<float>>();
	float3 rotation;
	rotation.x = values[0];
	rotation.y = values[1];
	rotation.z = values[2];

	SetRotation(rotation);

	values = json["size"].get<std::vector<float>>();
	float2 size;
	size.x = values[0];
	size.y = values[1];

	SetSize(size);

	values = json["pivot"].get<std::vector<float>>();
	float2 pivot;
	pivot.x = values[0];
	pivot.y = values[1];

	SetPivot(pivot);

	values = json["logicalSize"].get<std::vector<float>>();
	float2 logicalSize;
	logicalSize.x = values[0];
	logicalSize.y = values[1];

	SetLogicalSize(logicalSize);

	int anchor = json["anchor"].get<int>();
	SetAnchor((ComponentTransform2D::Anchor)anchor);
}

bool ComponentCanvas::Update(float dt)
{
	return true;
}

bool ComponentCanvas::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Canvas 2D"))
	{
		ImGui::Text("Size: %f, %f", owner->GetEngine()->GetEditor()->lastViewportSize.x, owner->GetEngine()->GetEditor()->lastViewportSize.y);
		ImGui::DragFloat2("Logical Size", &logicalSize[0]);
	}

	return true;
}

float2 ComponentCanvas::GetLogicalSize()
{
	return logicalSize;
}

float2 ComponentCanvas::GetNormalizedSize()
{
	return LogicalToViewport(logicalSize);
}

float2 ComponentCanvas::GetNormalizedPosition()
{
	return position;
}

float2 ComponentCanvas::LogicalToViewport(float2 vec) // GOOD
{
	return { vec.x / logicalSize.x * owner->GetEngine()->GetEditor()->lastViewportSize.x, vec.y / logicalSize.y * owner->GetEngine()->GetEditor()->lastViewportSize.y };
}

float2 ComponentCanvas::ViewportToLogical(float2 vec) // GOOD
{
	return { vec.x / owner->GetEngine()->GetEditor()->lastViewportSize.x * logicalSize.x, vec.y / owner->GetEngine()->GetEditor()->lastViewportSize.y * logicalSize.y};
}

float2 ComponentCanvas::ScreenToViewport(float2 vec)
{
	return { vec.x - owner->GetEngine()->GetEditor()->scenePanelOrigin.x, owner->GetEngine()->GetEditor()->lastViewportSize.y - (vec.y - owner->GetEngine()->GetEditor()->scenePanelOrigin.y) };
}

float2 ComponentCanvas::ScreenToLogical(float2 vec)
{
	float2 mouseViewportPosition = ScreenToViewport(vec);
	return { mouseViewportPosition.x / owner->GetEngine()->GetEditor()->lastViewportSize.x * logicalSize.x, mouseViewportPosition.y / owner->GetEngine()->GetEditor()->lastViewportSize.y * logicalSize.y };
}