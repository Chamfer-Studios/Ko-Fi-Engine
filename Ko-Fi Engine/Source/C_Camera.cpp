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
		cameraSpeed = 120.f;
		baseCameraSpeed = 120.f;
		speedMultiplier = 1;
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

		if (isFrustumCullingActive)
			FrustumCulling();

	}

	return true;
}

bool C_Camera::CleanUp()
{
	CONSOLE_LOG("Cleaning up the camera");
	appLog->AddLog("Cleaning up the camera\n");

	return true;
}

bool C_Camera::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true; // TODO: We don't need it to return a bool... Make it void when possible.

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;

		if (ImGui::DragFloat("Vertical fov", &verticalFOV))
		{
			isProjectionDirty = true;
		}
		if (ImGui::DragFloat("Near plane distance", &nearPlaneDistance))
		{
			isProjectionDirty = true;
		}
		if (ImGui::DragFloat("Far plane distance", &farPlaneDistance))
		{
			isProjectionDirty = true;
		}
		if (ImGui::Checkbox("Draw frustum", &isDrawFrustumActive))
		{
		}
		if (ImGui::Checkbox("Frustum culling", &isFrustumCullingActive))
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
	json["draw_frustum"] = isDrawFrustumActive;
	json["frustum_culling"] = isFrustumCullingActive;
	json["isMainCamera"] = isMainCamera;
}

void C_Camera::Load(Json& json)
{
	verticalFOV = json.at("vertical_fov");
	nearPlaneDistance = json.at("near_plane_distance");
	farPlaneDistance = json.at("far_plane_distance");
	isDrawFrustumActive = json.at("draw_frustum");
	isFrustumCullingActive = json.at("frustum_culling");
	isMainCamera = json.at("isMainCamera");
	if(isMainCamera)
		owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
}

float C_Camera::GetFarPlaneHeight() const
{
	return 2.0f * cameraFrustum.farPlaneDistance * Tan(cameraFrustum.verticalFov * 0.5f * DEGTORAD);
}

float C_Camera::GetFarPlaneWidth() const
{
	return GetFarPlaneHeight()*aspectRatio;
}

float4x4 C_Camera::GetViewMatrix() const
{
	return cameraFrustum.ViewMatrix();
}

void C_Camera::SetAspectRatio(const float& aspectRatio)
{
	cameraFrustum.horizontalFov = cameraFrustum.horizontalFov;
	cameraFrustum.verticalFov = 2.f * Atan(Tan(cameraFrustum.horizontalFov * 0.5 / aspectRatio));
	this->isProjectionDirty = true;
	RecalculateProjection();
}

void C_Camera::LookAt(const float3& point)
{
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

void C_Camera::ChangeSpeed(int multiplier)
{
	cameraSpeed = baseCameraSpeed * multiplier;
}

void C_Camera::CalculateViewMatrix(bool ortho)
{
	if (isProjectionDirty)
		RecalculateProjection(ortho);

	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
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

void C_Camera::DrawFrustum() const
{
	glPushMatrix();
	glMultMatrixf(this->owner->GetTransform()->GetGlobalTransform().Transposed().ptr());
	float3 cornerPoints[8];
	cameraFrustum.GetCornerPoints(cornerPoints);

	//Draw Operations
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.5f);
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

