#include "ComponentCanvas.h"
#include "Engine.h"
#include "Editor.h"
#include "UI.h"
#include "Window.h"
#include "GameObject.h"

#include "glew.h"

ComponentCanvas::ComponentCanvas(GameObject* parent) : ComponentTransform2D(parent)
{
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
	}

	return true;
}

float2 ComponentCanvas::GetNormalizedSize()
{
	return { owner->GetEngine()->GetEditor()->lastViewportSize.x, owner->GetEngine()->GetEditor()->lastViewportSize.y };
}

float2 ComponentCanvas::GetNormalizedPosition()
{
	return position;
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
