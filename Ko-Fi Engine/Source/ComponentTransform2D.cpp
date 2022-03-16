#include "ComponentTransform2D.h"

#include "Engine.h"
#include "Input.h"
#include "UI.h"
#include "Editor.h"

#include "GameObject.h"
#include "ComponentCanvas.h"
#include "Window.h"

#include "ImGuiAppLog.h"

#include "PanelChooser.h"

ComponentTransform2D::ComponentTransform2D(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRANSFORM2D;

	// Set default position, size, pivot, rotation & anchor
	position = { 0 , 0};
	size = { 200, 200 };
	pivot = { 0.5f, 0.5f };
	rotation = { 0.0f,0.f,0.f };
	anchor = Anchor::CENTER;
}

void ComponentTransform2D::Save(Json& json) const
{
	json["type"] = "transform2D";

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
	json["anchor"] = (int)GetAnchor();
}

void ComponentTransform2D::Load(Json& json)
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

	int anchor = json["anchor"].get<int>();
	SetAnchor((ComponentTransform2D::Anchor)anchor);
}

bool ComponentTransform2D::Update(float dt)
{
	return true;
}

bool ComponentTransform2D::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Transform 2D"))
	{
		float2 newPosition = GetPosition();
		if (ImGui::DragFloat2("Location", &newPosition[0])) // POSITION
		{
			SetPosition(newPosition);
		}
		float2 newPivot = GetPivot();
		if (ImGui::DragFloat2("Pivot", &newPivot[0], 0.005f, 0.0f, 1.0f)) // PIVOT
		{
			SetPivot(newPivot);
		}
		float3 newRotation = GetRotation();
		float3 newRotationDeg = float3(newRotation.x * RADTODEG, newRotation.y * RADTODEG, newRotation.z * RADTODEG);
		if (ImGui::DragFloat3("Rotation", &newRotationDeg[0], 1.0f, 0.f, 360.f)) // ROTATION
		{
			float3 newRotationRad = float3(newRotationDeg.x * DEGTORAD, newRotationDeg.y * DEGTORAD, newRotationDeg.z * DEGTORAD);
			SetRotation(newRotationRad);
		}
		float2 newSize = GetSize();
		if (ImGui::DragFloat2("Size", &newSize[0])) //SIZE
		{
			SetSize(newSize);
		}
		int newAnchor = (int)GetAnchor(); // ANCHOR
		if (ImGui::Combo("Anchor", &newAnchor, "TOP_LEFT\0TOP_CENTER\0TOP_RIGHT\0LEFT\0CENTER\0RIGHT\0BOTTOM_LEFT\0BOTTOM_CENTER\0BOTTOM_RIGHT\0\0"))
		{
			SetAnchor((Anchor)newAnchor);
		}
	}

	return true;
}

void ComponentTransform2D::SetPosition(const float2& newPosition)
{
	position = newPosition;
}
void ComponentTransform2D::SetPivot(const float2& newPivot)
{
	pivot = newPivot;
}
void ComponentTransform2D::SetRotation(const float3& newRotation)
{
	rotation = newRotation;
}
void ComponentTransform2D::SetSize(const float2& newSize)
{
	size = newSize;
}
void ComponentTransform2D::SetAnchor(const Anchor& newAnchor)
{
	anchor = newAnchor;
}

float2 ComponentTransform2D::GetNormalizedPosition(bool invertY)
{
	ComponentTransform2D* parentTransform = owner->GetParent()->GetComponent<ComponentTransform2D>();
	if (parentTransform == nullptr) return float2(position.x, position.y);

	float2 normalizedPosition = GetCanvas()->LogicalToViewport(position);
	normalizedPosition = normalizedPosition + parentTransform->GetAnchorPosition(anchor) - GetNormalizedPivotOffset(invertY);
	return float2(normalizedPosition.x, normalizedPosition.y);
}

float2 ComponentTransform2D::GetNormalizedSize(bool invertY)
{
	ComponentTransform2D* parentTransform = owner->GetParent()->GetComponent<ComponentTransform2D>();
	if (parentTransform == nullptr) return size;

	float2 normalizedSize = GetCanvas()->LogicalToViewport(size);
	if (invertY)
		normalizedSize.y = -normalizedSize.y;
	return normalizedSize;
}

float2 ComponentTransform2D::GetNormalizedPivotOffset(bool invertY)
{
	float2 normalizedSize = GetNormalizedSize(invertY);
	return { pivot.x * normalizedSize.x, pivot.y * normalizedSize.y };
}

float2 ComponentTransform2D::GetAnchorPosition(Anchor _anchor)
{
	float2 normalizedPosition = GetNormalizedPosition();
	float2 normalizedSize = GetNormalizedSize();

	switch (_anchor)
	{
	case ComponentTransform2D::Anchor::TOP_LEFT:
		return { normalizedPosition.x, normalizedPosition.y + normalizedSize.y };
		break;
	case ComponentTransform2D::Anchor::TOP_CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y + normalizedSize.y };

		break;
	case ComponentTransform2D::Anchor::TOP_RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y + normalizedSize.y };

		break;
	case ComponentTransform2D::Anchor::LEFT:
		return { normalizedPosition.x, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_LEFT:
		return { normalizedPosition.x, normalizedPosition.y };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y };

		break;
	default:
		break;
	}
	return { 0, 0 };
}

ComponentCanvas* ComponentTransform2D::GetCanvas()
{
	ComponentCanvas* canvas = owner->GetParent()->GetComponent<ComponentCanvas>();
	if (canvas != nullptr) return canvas;
	ComponentTransform2D* pTransform = owner->GetParent()->GetComponent<ComponentTransform2D>();
	if (pTransform != nullptr) return pTransform->GetCanvas();
	return nullptr;
}

float2 ComponentTransform2D::GetCanvasLogicalSize()
{
	ComponentCanvas* canvas = owner->GetParent()->GetComponent<ComponentCanvas>();
	if (canvas != nullptr) return canvas->GetLogicalSize();
	ComponentTransform2D* pTransform = owner->GetParent()->GetComponent<ComponentTransform2D>();
	if (pTransform != nullptr) return pTransform->GetCanvasLogicalSize();
	return { 0, 0 };
}

bool ComponentTransform2D::CheckPointWithinBounds(float2 vec)
{
	float2 normalizedPosition = GetNormalizedPosition();
	float2 normalizedSize = GetNormalizedSize();

	float2 logicalPosition = GetCanvas()->ViewportToLogical(normalizedPosition);
	float2 logicalSize = GetCanvas()->ViewportToLogical(normalizedSize);

	float2 lowerLeft = { logicalPosition.x, logicalPosition.y };
	float2 upperRight = { logicalPosition.x + size.x, logicalPosition.y + size.y };

	return lowerLeft.x < vec.x && lowerLeft.y < vec.y && vec.x < upperRight.x && vec.y < upperRight.y;
}

/*void ComponentTransform2D::OnSave(JSONWriter& writer) const
{
	writer.String("Transform 2D");
	writer.StartObject();

	// Saving position
	writer.String("Position");
	writer.StartArray();
	writer.Double(GetPosition().x);
	writer.Double(GetPosition().y);
	writer.EndArray();

	// Saving pivot
	writer.String("Pivot");
	writer.StartArray();
	writer.Double(GetPivot().x);
	writer.Double(GetPivot().y);
	writer.EndArray();

	// Saving rotation
	writer.String("Rotation");
	writer.StartArray();
	//writer.Double(GetRotation());
	writer.EndArray();

	// Saving size
	writer.String("Size");
	writer.StartArray();
	writer.Double(GetSize().x);
	writer.Double(GetSize().y);
	writer.EndArray();

	// Saving anchor
	writer.String("Anchor");
	writer.StartArray();
	writer.Int((int)GetAnchor());
	writer.EndArray();
	writer.EndObject();
}

void ComponentTransform2D::OnLoad(const JSONReader& reader)
{
	// Loading position
	if (reader.HasMember("Position"))
	{
		const rapidjson::Value& itemPosition = reader["Position"];
		float positionX = 0.0f;
		float positionY = 0.0f;

		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemPosition.Begin(); it != itemPosition.End(); ++it)
		{
			if (i == 0) positionX = it->GetDouble();
			else if (i == 1) positionY = it->GetDouble();
			i++;
		}
		SetPosition(float2(positionX, positionY));
	}

	// Loading pivot
	if (reader.HasMember("Pivot"))
	{
		const rapidjson::Value& itemPivot = reader["Pivot"];
		float pivotX = 0.0f;
		float pivotY = 0.0f;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemPivot.Begin(); it != itemPivot.End(); ++it)
		{
			if (i == 0) pivotX = it->GetDouble();
			else if (i == 1) pivotY = it->GetDouble();
			i++;
		}
		SetPivot(float2(pivotX, pivotY));
	}

	// Loading rotation
	if (reader.HasMember("Rotation"))
	{
		const rapidjson::Value& itemRotation = reader["Rotation"];
		float newRotation;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemRotation.Begin(); it != itemRotation.End(); ++it)
		{
			if (i == 0) newRotation = it->GetDouble();
			i++;
		}
		//SetRotation(newRotation);
	}

	// Loading size
	if (reader.HasMember("Size"))
	{
		const rapidjson::Value& itemSize = reader["Size"];
		float sizeX = 0.0f;
		float sizeY = 0.0f;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemSize.Begin(); it != itemSize.End(); ++it)
		{
			if (i == 0) sizeX = it->GetDouble();
			else if (i == 1) sizeY = it->GetDouble();
			i++;
		}
		SetSize(float2(sizeX, sizeY));
	}

	// Loading anchor
	if (reader.HasMember("Anchor"))
	{
		const rapidjson::Value& itemAnchor = reader["Anchor"];
		int newAnchor = 0;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemAnchor.Begin(); it != itemAnchor.End(); ++it)
		{
			if (i == 0) newAnchor = it->GetInt();
			i++;
		}
		SetAnchor((Anchor)newAnchor);
	}
}*/