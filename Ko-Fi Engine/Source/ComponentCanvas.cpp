#include "ComponentCanvas.h"
#include "Engine.h"
#include "Editor.h"
#include "UI.h"

#include "glew.h"

ComponentCanvas::ComponentCanvas(KoFiEngine* engine, GameObject* parent) : ComponentTransform2D(engine, parent)
{
}

bool ComponentCanvas::Update()
{
	AdjustPositionAndSize();

	return true;
}

bool ComponentCanvas::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Canvas 2D"))
	{
		ImGui::Text("Size: %f, %f", size.x, size.y);
	}

	return true;
}

void ComponentCanvas::AdjustPositionAndSize()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float propX = viewport[2] / engine->GetEditor()->lastViewportSize.x;
	float propY = viewport[3] / engine->GetEditor()->lastViewportSize.y;

	position.x = 0;
	position.y = 0;
	size.x = engine->GetEditor()->lastViewportSize.x * propX;
	size.y = engine->GetEditor()->lastViewportSize.y * propY;
}

void ComponentCanvas::GetRealPosition(float2& realPosition, bool ignoreCanvas)
{
	float propX = engine->GetUI()->uiCameraViewport[2] / engine->GetEditor()->lastViewportSize.x;
	float propY = engine->GetUI()->uiCameraViewport[3] / engine->GetEditor()->lastViewportSize.y;

	realPosition.x = 0;
	realPosition.y = 0;
}

void ComponentCanvas::GetRealSize(float2& realSize)
{
	float propX = engine->GetUI()->uiCameraViewport[2] / engine->GetEditor()->lastViewportSize.x;
	float propY = engine->GetUI()->uiCameraViewport[3] / engine->GetEditor()->lastViewportSize.y;

	realSize.x = engine->GetEditor()->lastViewportSize.x * propX;
	realSize.y = engine->GetEditor()->lastViewportSize.y * propY;
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
