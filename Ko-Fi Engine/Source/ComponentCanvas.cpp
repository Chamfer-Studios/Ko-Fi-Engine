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

float2 ComponentCanvas::GetNormalizedSize(bool invertY)
{
	return LogicalToViewport(logicalSize);
}

float2 ComponentCanvas::GetNormalizedPosition(bool invertY)
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

//Json SceneManager::SaveComponentCanvas(ComponentCanvas* componentCanvas)
//{
//	Json jsonComponentTransform2D;
//	jsonComponentTransform2D["position"] = {
//		componentCanvas->GetPosition().x,
//		componentCanvas->GetPosition().y,
//	};
//	jsonComponentTransform2D["rotation"] = {
//		componentCanvas->GetRotation().x,
//		componentCanvas->GetRotation().y,
//		componentCanvas->GetRotation().z,
//	};
//	jsonComponentTransform2D["size"] = {
//		componentCanvas->GetSize().x,
//		componentCanvas->GetSize().y
//	};
//	jsonComponentTransform2D["pivot"] = {
//		componentCanvas->GetPivot().x,
//		componentCanvas->GetPivot().y
//	};
//	jsonComponentTransform2D["logicalSize"] = {
//		componentCanvas->GetLogicalSize().x,
//		componentCanvas->GetLogicalSize().y
//	};
//	jsonComponentTransform2D["anchor"] = (int)componentCanvas->GetAnchor();
//
//	return jsonComponentTransform2D;
//}

//void SceneManager::LoadComponentCanvas(ComponentCanvas* componentCanvas, Json jsonComponentCanvas)
//{
//	std::vector<float> values = jsonComponentCanvas["position"].get<std::vector<float>>();
//	float2 position;
//	position.x = values[0];
//	position.y = values[1];
//
//	componentCanvas->SetPosition(position);
//
//	values = jsonComponentCanvas["rotation"].get<std::vector<float>>();
//	float3 rotation;
//	rotation.x = values[0];
//	rotation.y = values[1];
//	rotation.z = values[2];
//
//	componentCanvas->SetRotation(rotation);
//
//	values = jsonComponentCanvas["size"].get<std::vector<float>>();
//	float2 size;
//	size.x = values[0];
//	size.y = values[1];
//
//	componentCanvas->SetSize(size);
//
//	values = jsonComponentCanvas["pivot"].get<std::vector<float>>();
//	float2 pivot;
//	pivot.x = values[0];
//	pivot.y = values[1];
//
//	componentCanvas->SetPivot(pivot);
//
//	values = jsonComponentCanvas["logicalSize"].get<std::vector<float>>();
//	float2 logicalSize;
//	logicalSize.x = values[0];
//	logicalSize.y = values[1];
//
//	componentCanvas->SetLogicalSize(logicalSize);
//
//	int anchor = jsonComponentCanvas["anchor"].get<int>();
//	componentCanvas->SetAnchor((ComponentTransform2D::Anchor)anchor);
//}