#include "C_Transform2D.h"

#include "Engine.h"
#include "M_Input.h"
#include "M_UI.h"
#include "M_Editor.h"

#include "GameObject.h"
#include "C_Canvas.h"
#include "M_Window.h"

#include "ImGuiAppLog.h"

#include "PanelChooser.h"

C_Transform2D::C_Transform2D(GameObject* parent) : Component(parent)
{
	type = ComponentType::TRANSFORM2D;

	// Set default position, size, pivot, rotation & anchor
	position = { 0 , 0};
	size = { 200, 200 };
	pivot = { 0.5f, 0.5f };
	rotation = { 0.0f,0.f,0.f };
	anchor = Anchor::CENTER;
	drawablePlane = new MyPlane(owner);
}

C_Transform2D::~C_Transform2D()
{
	delete drawablePlane;
}

bool C_Transform2D::CleanUp()
{
	return true;
}

void C_Transform2D::Save(Json& json) const
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
	json["anchor"] = (int)GetAnchor();
}

void C_Transform2D::Load(Json& json)
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
	SetAnchor((C_Transform2D::Anchor)anchor);
}

bool C_Transform2D::Update(float dt)
{
	return true;
}

bool C_Transform2D::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("Transform 2D", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;

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

		if (ImGui::DragFloat2("Mask", &mask[0], 0.005f, 0.0f, 1.0f)) {
			drawablePlane->texCoords.clear();
			drawablePlane->texCoords.shrink_to_fit();
			drawablePlane->texCoords.push_back({ 0, mask.y });
			drawablePlane->texCoords.push_back({ 0, 0 });
			drawablePlane->texCoords.push_back({ mask.x, mask.y });
			drawablePlane->texCoords.push_back({ mask.x, 0 });

			glBindBuffer(GL_ARRAY_BUFFER, drawablePlane->textureBufferId);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * drawablePlane->texCoords.size(), &drawablePlane->texCoords[0], GL_STATIC_DRAW);
		}
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}

void C_Transform2D::SetPosition(const float2& newPosition)
{
	position = newPosition;
}
void C_Transform2D::SetPivot(const float2& newPivot)
{
	pivot = newPivot;
}
void C_Transform2D::SetRotation(const float3& newRotation)
{
	rotation = newRotation;
}
void C_Transform2D::SetSize(const float2& newSize)
{
	size = newSize;
}
void C_Transform2D::SetAnchor(const Anchor& newAnchor)
{
	anchor = newAnchor;
}

void C_Transform2D::SetMask(const float2& newMask)
{
	this->mask = newMask;
}

float2 C_Transform2D::GetNormalizedPosition()
{
	C_Transform2D* parentTransform = owner->GetParent()->GetComponent<C_Transform2D>();
	if (parentTransform == nullptr) return float2(position.x, position.y);

	float2 normalizedPosition = GetCanvas()->LogicalToViewport(position);
	normalizedPosition = normalizedPosition + parentTransform->GetAnchorPosition(anchor) - GetNormalizedPivotOffset();

	float2 normalizedSize = GetCanvas()->LogicalToViewport(size);
	return float2(normalizedPosition.x - (1.0f - mask.x) * (normalizedSize.x * pivot.x), normalizedPosition.y - (1.0f - mask.y) * (normalizedSize.y * pivot.y)); // VODOO
}

float2 C_Transform2D::GetNormalizedSize()
{
	C_Transform2D* parentTransform = owner->GetParent()->GetComponent<C_Transform2D>();
	if (parentTransform == nullptr) return size;

	float2 normalizedSize = GetCanvas()->LogicalToViewport(size);
	return { normalizedSize.x * mask.x, normalizedSize.y * mask.y };
}

float2 C_Transform2D::GetNormalizedPivotOffset()
{
	float2 normalizedSize = GetNormalizedSize();
	return { pivot.x * normalizedSize.x, pivot.y * normalizedSize.y };
}

float2 C_Transform2D::GetAnchorPosition(Anchor _anchor)
{
	float2 normalizedPosition = GetNormalizedPosition();
	float2 normalizedSize = GetNormalizedSize();

	switch (_anchor)
	{
	case C_Transform2D::Anchor::TOP_LEFT:
		return { normalizedPosition.x, normalizedPosition.y + normalizedSize.y };
		break;
	case C_Transform2D::Anchor::TOP_CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y + normalizedSize.y };

		break;
	case C_Transform2D::Anchor::TOP_RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y + normalizedSize.y };

		break;
	case C_Transform2D::Anchor::LEFT:
		return { normalizedPosition.x, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case C_Transform2D::Anchor::CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case C_Transform2D::Anchor::RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y + normalizedSize.y / 2 };

		break;
	case C_Transform2D::Anchor::BOTTOM_LEFT:
		return { normalizedPosition.x, normalizedPosition.y };

		break;
	case C_Transform2D::Anchor::BOTTOM_CENTER:
		return { normalizedPosition.x + normalizedSize.x / 2, normalizedPosition.y };

		break;
	case C_Transform2D::Anchor::BOTTOM_RIGHT:
		return { normalizedPosition.x + normalizedSize.x, normalizedPosition.y };

		break;
	default:
		break;
	}
	return { 0, 0 };
}

C_Canvas* C_Transform2D::GetCanvas()
{
	C_Canvas* canvas = owner->GetParent()->GetComponent<C_Canvas>();
	if (canvas != nullptr) return canvas;
	C_Transform2D* pTransform = owner->GetParent()->GetComponent<C_Transform2D>();
	if (pTransform != nullptr) return pTransform->GetCanvas();
	return nullptr;
}

float2 C_Transform2D::GetCanvasLogicalSize()
{
	C_Canvas* canvas = owner->GetParent()->GetComponent<C_Canvas>();
	if (canvas != nullptr) return canvas->GetLogicalSize();
	C_Transform2D* pTransform = owner->GetParent()->GetComponent<C_Transform2D>();
	if (pTransform != nullptr) return pTransform->GetCanvasLogicalSize();
	return { 0, 0 };
}

bool C_Transform2D::CheckPointWithinBounds(float2 vec)
{
	float2 normalizedPosition = GetNormalizedPosition();
	float2 normalizedSize = GetNormalizedSize();

	float2 logicalPosition = GetCanvas()->ViewportToLogical(normalizedPosition);
	float2 logicalSize = GetCanvas()->ViewportToLogical(normalizedSize);

	float2 lowerLeft = { logicalPosition.x, logicalPosition.y };
	float2 upperRight = { logicalPosition.x + size.x, logicalPosition.y + size.y };

	return lowerLeft.x < vec.x && lowerLeft.y < vec.y && vec.x < upperRight.x && vec.y < upperRight.y;
}