#include "C_Camera.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"
#include "M_Camera3D.h"
#include "M_Editor.h"

// GameObject
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"

#include "Globals.h"
#include "Log.h"
#include "ImGuiAppLog.h"

#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/Math/MathFunc.h"

#include "optick.h"

C_Camera::C_Camera(GameObject* parent, bool isEngineCamera) : Component(parent)
{
	this->isEngineCamera = isEngineCamera;
	type = ComponentType::CAMERA;

	if (isEngineCamera)
	{
		aspectRatio = 1.f;
		verticalFOV = 60.f;
		nearPlaneDistance = 0.1f;
		farPlaneDistance = 5000.f;
		cameraSensitivity = .1f;
		cameraSpeed = 30.f;
	}
	if (!isEngineCamera)
		componentTransform = owner->GetTransform();

	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -15.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	LookAt(float3::zero);

	CalculateViewMatrix();

}

C_Camera::~C_Camera()
{
}

bool C_Camera::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	LookAt(float3::zero);

	bool ret = true;

	return ret;
}

bool C_Camera::CleanUp()
{
	CONSOLE_LOG("Cleaning up the camera");
	appLog->AddLog("Cleaning up the camera\n");

	return true;
}

bool C_Camera::Update(float dt)
{
	// Add update functionality when we are able to change the main camera.
	if (!isEngineCamera)
	{
		position = componentTransform->GetPosition();

		up = componentTransform->Up();
		front = componentTransform->Front();
		//right = componentTransform->Right();

		CalculateViewMatrix();

		if (frustumCulling)
			FrustumCulling();

	}

	return true;
}

void C_Camera::LookAt(const float3& point)
{
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

void C_Camera::CalculateViewMatrix(bool ortho)
{
	if (projectionIsDirty)
		RecalculateProjection(ortho);

	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void C_Camera::RecalculateProjection(bool ortho)
{
	if (!ortho ) {
		cameraFrustum.type = FrustumType::PerspectiveFrustum;
		cameraFrustum.nearPlaneDistance = nearPlaneDistance;
		cameraFrustum.farPlaneDistance = farPlaneDistance;
		cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
		cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
	}
	else {
		if (owner)
		{
			cameraFrustum.type = FrustumType::OrthographicFrustum;
			cameraFrustum.nearPlaneDistance = nearPlaneDistance;
			cameraFrustum.farPlaneDistance = farPlaneDistance;
			cameraFrustum.orthographicWidth = owner->GetEngine()->GetEditor()->lastViewportSize.x;
			cameraFrustum.orthographicHeight = owner->GetEngine()->GetEditor()->lastViewportSize.y;
		}
		
	}
}

bool C_Camera::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		if (ImGui::DragFloat("Vertical fov", &verticalFOV))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Near plane distance", &nearPlaneDistance))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Far plane distance", &farPlaneDistance))
		{
			projectionIsDirty = true;
		}
		if (ImGui::Checkbox("Draw frustum", &drawFrustum))
		{
		}
		if (ImGui::Checkbox("Frustum culling", &frustumCulling))
		{
			ResetFrustumCulling();
		}
		if (ImGui::Checkbox("Set As Main Camera", &isMainCamera))
		{
			owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
		}
	}
	else
		DrawDeleteButton(owner, this);
	return ret;
}

void C_Camera::Save(Json& json) const
{
	json["type"] = "camera";
	json["vertical_fov"] = verticalFOV;
	json["near_plane_distance"] = nearPlaneDistance;
	json["far_plane_distance"] = farPlaneDistance;
	json["draw_frustum"] = drawFrustum;
	json["frustum_culling"] = frustumCulling;
	json["isMainCamera"] = isMainCamera;
}

void C_Camera::Load(Json& json)
{
	verticalFOV = json.at("vertical_fov");
	nearPlaneDistance = json.at("near_plane_distance");
	farPlaneDistance = json.at("far_plane_distance");
	drawFrustum = json.at("draw_frustum");
	frustumCulling = json.at("frustum_culling");
	isMainCamera = json.at("isMainCamera");
	if(isMainCamera)
		owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
}

//void ModuleCamera3D::OnSave(JSONWriter& writer) const
//{
//	writer.String("camera");
//	writer.StartObject();
//	SAVE_JSON_FLOAT(verticalFOV)
//		SAVE_JSON_FLOAT(nearPlaneDistance)
//		SAVE_JSON_FLOAT(farPlaneDistance)
//		SAVE_JSON_FLOAT(cameraSpeed)
//		SAVE_JSON_FLOAT(cameraSensitivity)
//		writer.EndObject();
//}

//void ModuleCamera3D::OnLoad(const JSONReader& reader)
//{
//	if (reader.HasMember("camera"))
//	{
//		const auto& config = reader["camera"];
//		LOAD_JSON_FLOAT(verticalFOV);
//		LOAD_JSON_FLOAT(nearPlaneDistance);
//		LOAD_JSON_FLOAT(farPlaneDistance);
//		LOAD_JSON_FLOAT(cameraSpeed);
//		LOAD_JSON_FLOAT(cameraSensitivity);
//	}
//	RecalculateProjection();
//}

void C_Camera::DrawFrustum() const
{
	OPTICK_EVENT();

	glPushMatrix();
	glMultMatrixf(this->owner->GetTransform()->GetGlobalTransform().Transposed().ptr());
	float3 cornerPoints[8];
	cameraFrustum.GetCornerPoints(cornerPoints);

	//Draw Operations

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(3.5f);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glPopMatrix();

}

bool C_Camera::ClipsWithBBox(const AABB& refBox) const
{
	float3 vertexCorner[8];

	// Get bounding box
	refBox.GetCornerPoints(vertexCorner);

	for (int p = 0; p < 6; ++p)
	{
		int cornersOutside = 8;

		for (int i = 0; i < 8; ++i)
		{
			if (cameraFrustum.GetPlane(p).IsOnPositiveSide(vertexCorner[i])) --cornersOutside;
		}

		if (cornersOutside == 0) return false;
	}
}

void C_Camera::FrustumCulling()
{
	std::vector<GameObject*> gameObjects = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList;

	for (std::vector<GameObject*>::iterator go = gameObjects.begin(); go != gameObjects.end(); go++)
	{
		GameObject* gameObject = (*go);
		C_Mesh* componentMesh = gameObject->GetComponent<C_Mesh>();

		if (componentMesh == nullptr || gameObject == owner)
			continue;

		if (!ClipsWithBBox(componentMesh->GetGlobalAABB()))
			componentMesh->SetRenderMesh(false);
		else
			componentMesh->SetRenderMesh(true);
	}
}

void C_Camera::ResetFrustumCulling()
{
	std::vector<GameObject*> gameObjects = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList;

	for (std::vector<GameObject*>::iterator go = gameObjects.begin(); go != gameObjects.end(); go++)
	{
		GameObject* gameObject = (*go);
		C_Mesh* componentMesh = gameObject->GetComponent<C_Mesh>();

		if (componentMesh == nullptr || gameObject == owner)
			continue;

		if (!ClipsWithBBox(componentMesh->GetGlobalAABB()))
			componentMesh->SetRenderMesh(true);
	}
}

void C_Camera::SetAspectRatio(const float& aspectRatio)
{
	cameraFrustum.horizontalFov = cameraFrustum.horizontalFov;
	cameraFrustum.verticalFov = 2.f * Atan(Tan(cameraFrustum.horizontalFov * 0.5 / aspectRatio));
	this->projectionIsDirty = true;
	RecalculateProjection();
}
