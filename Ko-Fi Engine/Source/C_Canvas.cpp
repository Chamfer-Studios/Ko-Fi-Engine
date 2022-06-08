#include "C_Canvas.h"
#include "Engine.h"
#include "M_Editor.h"
#include "M_UI.h"
#include "M_Window.h"
#include "GameObject.h"

#include "glew.h"

C_Canvas::C_Canvas(GameObject* parent) : C_Transform2D(parent)
{
	type = ComponentType::CANVAS;
	typeIndex = typeid(*this);

	engine = parent->GetEngine();

	logicalSize = { 1920, 1080 };
}

C_Canvas::~C_Canvas()
{}

bool C_Canvas::CleanUp()
{
	return true;
}

void C_Canvas::Save(Json& json) const
{
	json["type"] = (int)type;

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

void C_Canvas::Load(Json& json)
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
	SetAnchor((C_Transform2D::Anchor)anchor);
}

bool C_Canvas::Update(float dt)
{
	return true;
}

bool C_Canvas::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Canvas 2D", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;

		ImGui::Text("Size: %f, %f", owner->GetEngine()->GetEditor()->lastViewportSize.x, owner->GetEngine()->GetEditor()->lastViewportSize.y);
		ImGui::DragFloat2("Logical Size", &logicalSize[0]);
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}

float2 C_Canvas::GetLogicalSize()
{
	return logicalSize;
}

float2 C_Canvas::GetNormalizedSize()
{
	return LogicalToViewport(logicalSize);
}

float2 C_Canvas::GetNormalizedPosition()
{
	return position;
}

float2 C_Canvas::LogicalToViewport(float2 vec) // GOOD
{
	return { vec.x / logicalSize.x * owner->GetEngine()->GetEditor()->lastViewportSize.x, vec.y / logicalSize.y * owner->GetEngine()->GetEditor()->lastViewportSize.y };
}

float2 C_Canvas::ViewportToLogical(float2 vec) // GOOD
{
	return { vec.x / owner->GetEngine()->GetEditor()->lastViewportSize.x * logicalSize.x, vec.y / owner->GetEngine()->GetEditor()->lastViewportSize.y * logicalSize.y};
}

float2 C_Canvas::ScreenToViewport(float2 vec)
{
	return { vec.x - engine->GetEditor()->scenePanelOrigin.x, engine->GetEditor()->lastViewportSize.y - (vec.y - engine->GetEditor()->scenePanelOrigin.y) };
}

float2 C_Canvas::ScreenToLogical(float2 vec)
{
	float2 mouseViewportPosition = ScreenToViewport(vec);
	return { mouseViewportPosition.x / owner->GetEngine()->GetEditor()->lastViewportSize.x * logicalSize.x, mouseViewportPosition.y / owner->GetEngine()->GetEditor()->lastViewportSize.y * logicalSize.y };
}