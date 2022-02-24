#include "ComponentTransform2D.h"

#include "Engine.h"
#include "Input.h"
#include "UI.h"
#include "Editor.h"

#include "GameObject.h"
#include "ComponentCanvas.h"

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

bool ComponentTransform2D::Update()
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
		if (ImGui::DragFloat2("Pivot", &newPivot[0])) // PIVOT
		{
			SetPivot(newPivot);
		}
		float3 newRotation = GetRotation();
		if (ImGui::DragFloat3("Rotation", &newRotation[0], 0.5f, 0.f, 360.f)) // ROTATION
		{
			SetRotation(newRotation);
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
	// TODO: UPDATEAR LA MATRIZ
	rotation = newRotation;
	//owner->GetComponent<ComponentImage>().
}
void ComponentTransform2D::SetSize(const float2& newSize)
{
	size = newSize;
}
void ComponentTransform2D::SetAnchor(const Anchor& newAnchor)
{
	anchor = newAnchor;
	//CalculateAnchor();
}

void ComponentTransform2D::GetRealPosition(float2& realPosition, bool ignoreCanvas)
{
	ComponentTransform2D* parentTransform = (ComponentTransform2D*)owner->GetParent()->GetComponent<ComponentCanvas>();
	if (parentTransform != nullptr) {
		if (ignoreCanvas)
		{
			realPosition = parentTransform->GetRelativeAnchorPosition(anchor) + position;
			return;
		}
	}
	else {
		parentTransform = owner->GetParent()->GetComponent<ComponentTransform2D>();
	}

	if (parentTransform == nullptr) return;

	float2 anchorPosition = parentTransform->GetAnchorPosition(anchor);
	realPosition = anchorPosition + position;
	//realPosition.x += owner->GetEngine()->GetEditor()->scenePanelOrigin.x;
	//realPosition.y += owner->GetEngine()->GetEditor()->scenePanelOrigin.y;
}

void ComponentTransform2D::GetRealSize(float2& realSize)
{
	float propX = owner->GetEngine()->GetUI()->uiCameraViewport[2] / (owner->GetEngine()->GetEditor()->lastViewportSize.x);
	float propY = owner->GetEngine()->GetUI()->uiCameraViewport[3] / (owner->GetEngine()->GetEditor()->lastViewportSize.y);

	realSize.x = size.x;
	realSize.y = size.y;
	realSize.x /= owner->GetEngine()->GetEditor()->lastViewportSize.x;
	realSize.y /= owner->GetEngine()->GetEditor()->lastViewportSize.y;
}

float2 ComponentTransform2D::GetCanvasCenter()
{
	ComponentCanvas* canvas = owner->GetParent()->GetComponent<ComponentCanvas>();
	if (canvas != nullptr)
		return canvas->GetAnchorPosition(Anchor::CENTER);
	else
	{
		ComponentTransform2D* trans = owner->GetParent()->GetComponent<ComponentTransform2D>();
		if (trans == nullptr) return { 0, 0 };
		else return trans->GetCanvasCenter();
	}
}

bool ComponentTransform2D::CheckMouseInsideBounds()
{
	float2 mousePosition = { (float)owner->GetEngine()->GetInput()->GetMouseX(), SCREEN_HEIGHT - (float)owner->GetEngine()->GetInput()->GetMouseY()};

	float2 uiMousePosition = owner->GetEngine()->GetUI()->GetUINormalizedMousePosition();
	uiMousePosition.x = uiMousePosition.x / owner->GetEngine()->GetEditor()->lastViewportSize.x * SCREEN_WIDTH;
	uiMousePosition.y = uiMousePosition.y / owner->GetEngine()->GetEditor()->lastViewportSize.y * SCREEN_HEIGHT;

	float2 realPos;
	GetRealPosition(realPos, false);

	float2 realSize;
	GetRealSize(realSize);

	float2 lowerBot = { realPos.x - realSize.x / 2, realPos.y - realSize.y / 2 };
	float2 upperRight = { realPos.x + realSize.x / 2, realPos.y + realSize.y / 2 };

	//TODO CHECK
	if (lowerBot.x < uiMousePosition.x && upperRight.x > uiMousePosition.x)
	{
		if (lowerBot.y < uiMousePosition.y && upperRight.y > uiMousePosition.y)
		{
			return true;
		}
	}

	return false;
}

float2 ComponentTransform2D::GetAnchorPosition(Anchor anchor)
{
	float2 realPosition;
	GetRealPosition(realPosition);
	float2 realSize;
	GetRealSize(realSize);

	switch (anchor)
	{
	case ComponentTransform2D::Anchor::TOP_LEFT:
		return { realPosition.x, realPosition.y + realSize.y };
		break;
	case ComponentTransform2D::Anchor::TOP_CENTER:
		return { realPosition.x + realSize.x / 2, realPosition.y + realSize.y };

		break;
	case ComponentTransform2D::Anchor::TOP_RIGHT:
		return { realPosition.x + realSize.x, realPosition.y + realSize.y };

		break;
	case ComponentTransform2D::Anchor::LEFT:
		return { realPosition.x, realPosition.y + realSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::CENTER:
		return { realPosition.x + realSize.x / 2, realPosition.y + realSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::RIGHT:
		return { realPosition.x + realSize.x, realPosition.y + realSize.y / 2 };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_LEFT:
		return { realPosition.x, realPosition.y };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_CENTER:
		return { realPosition.x + realSize.x / 2, realPosition.y };

		break;
	case ComponentTransform2D::Anchor::BOTTOM_RIGHT:
		return { realPosition.x + realSize.x, realPosition.y };

		break;
	default:
		break;
	}
	return { 0, 0 };
}

float2 ComponentTransform2D::GetRelativeAnchorPosition(Anchor anchor)
{
	float2 realPosition;
	GetRealPosition(realPosition);
	float2 realSize;
	GetRealSize(realSize);

	float2 anchorPosition = GetAnchorPosition(anchor);
	float2 bottomLeft = { realPosition.x + owner->GetEngine()->GetEditor()->scenePanelOrigin.x, realPosition.y + owner->GetEngine()->GetEditor()->scenePanelOrigin.y };

	return anchorPosition - bottomLeft;
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


