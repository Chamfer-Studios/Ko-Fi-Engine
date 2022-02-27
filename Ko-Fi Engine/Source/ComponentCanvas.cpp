#include "ComponentCanvas.h"
#include "Engine.h"
#include "Editor.h"
#include "UI.h"
#include "Window.h"
#include "GameObject.h"

#include "glew.h"

ComponentCanvas::ComponentCanvas(GameObject* parent) : ComponentTransform2D(parent)
{
	logicalSize = { 1920, 1080 };
}

bool ComponentCanvas::Update()
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

float2 ComponentCanvas::LogicalToViewport(float2 vec)
{
	return { vec.x / logicalSize.x * owner->GetEngine()->GetWindow()->GetWidth(), vec.y / logicalSize.y * owner->GetEngine()->GetWindow()->GetHeight() };
}

float2 ComponentCanvas::ViewportToLogical(float2 vec)
{
	return { vec.x / owner->GetEngine()->GetWindow()->GetWidth() * logicalSize.x, vec.y / owner->GetEngine()->GetWindow()->GetHeight() * logicalSize.y};
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

/*void ComponentCanvas::OnLoad(const JSONReader& reader)
{
}

void ComponentCanvas::OnSave(JSONWriter& writer) const
{
	writer.String("Canvas");
	writer.StartObject();

	// Saving button state

	writer.EndObject();
}*/
