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
		ImGui::Text("Size: %f, %f", GetNormalizedSize().x, GetNormalizedSize().y);
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
	return LogicalToScreen(logicalSize);
}

float2 ComponentCanvas::GetNormalizedPosition()
{
	return position;
}

float2 ComponentCanvas::LogicalToScreen(float2 vec)
{
	return { vec.x / logicalSize.x * owner->GetEngine()->GetWindow()->GetWidth(), vec.y / logicalSize.y * owner->GetEngine()->GetWindow()->GetHeight() };
}

float2 ComponentCanvas::ScreenToLogical(float2 vec)
{
	return { vec.x / owner->GetEngine()->GetWindow()->GetWidth() * logicalSize.x, vec.y / owner->GetEngine()->GetWindow()->GetHeight() * logicalSize.y };
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
