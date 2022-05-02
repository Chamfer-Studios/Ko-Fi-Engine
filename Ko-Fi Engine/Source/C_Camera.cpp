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
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Geometry/Plane.h"

#include "optick.h"

C_Camera::C_Camera(GameObject* parent, bool isEngineCamera) : Component(parent)
{
	this->isEngineCamera = isEngineCamera;
	type = ComponentType::CAMERA;
	//if its not engine camera, get the owner's transform
	if (!isEngineCamera)
		componentTransform = owner->GetTransform();
	//Set Default Values for the component
	aspectRatio = 1.778f;
	cameraSensitivity = .1f;
	cameraSpeed = 120.f;
	baseCameraSpeed = 120.f;
	speedMultiplier = 1;
	//Create the frustum
	cameraFrustum = Frustum();
	//Set Default Values for the frusum
	cameraFrustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumLeftHanded);
	cameraFrustum.SetPerspective(DegToRad(43.0f), DegToRad(22.0f));
	cameraFrustum.SetHorizontalFovAndAspectRatio(DegToRad(45.0f),aspectRatio);
	cameraFrustum.SetViewPlaneDistances(0.01f, 1000.0f);
	cameraFrustum.SetFrame(float3(0.0f,0.0f,20.0f),float3(0.0f,0.0f,1.0f),float3(0.0f,1.0f,0.0f));

	reference = float3(0.0f, 0.0f, 0.0f);

	LookAt(float3::zero);

}

C_Camera::~C_Camera()
{
}

bool C_Camera::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	bool ret = true;

	return ret;
}

bool C_Camera::Update(float dt)
{
	// Add update functionality when we are able to change the main camera.
	if (!isEngineCamera)
	{

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
		DrawDeleteButton(owner, this);

		float verticalFOV = cameraFrustum.VerticalFov();
		if (ImGui::DragFloat("Vertical fov", &verticalFOV))
		{
			cameraFrustum.SetVerticalFovAndAspectRatio(verticalFOV, aspectRatio);
		}
		float2 planeDistances = { cameraFrustum.NearPlaneDistance(),cameraFrustum.FarPlaneDistance() };
		if (ImGui::DragFloat2("Near & Far plane distances", &(planeDistances[0])))
		{
			cameraFrustum.SetViewPlaneDistances(planeDistances.x, planeDistances.y);
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
	json["vertical_fov"] = cameraFrustum.VerticalFov();
	json["near_plane_distance"] = cameraFrustum.NearPlaneDistance();
	json["far_plane_distance"] = cameraFrustum.FarPlaneDistance();
	json["draw_frustum"] = isDrawFrustumActive;
	json["frustum_culling"] = isFrustumCullingActive;
	json["isMainCamera"] = isMainCamera;
}

void C_Camera::Load(Json& json)
{
	cameraFrustum.SetVerticalFovAndAspectRatio(json.at("vertical_fov"),1.778f);
	cameraFrustum.SetViewPlaneDistances(json.at("near_plane_distance"), json.at("far_plane_distance"));
	isDrawFrustumActive = json.at("draw_frustum");
	isFrustumCullingActive = json.at("frustum_culling");
	isMainCamera = json.at("isMainCamera");
	if (isMainCamera)
		owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
}

float C_Camera::GetFarPlaneHeight() const
{
	return 2.0f * cameraFrustum.FarPlaneDistance() * Tan(cameraFrustum.VerticalFov() * 0.5f * DEGTORAD);
}

float C_Camera::GetFarPlaneWidth() const
{
	return GetFarPlaneHeight() * aspectRatio;
}

float4x4 C_Camera::GetViewMatrix() const
{
	return cameraFrustum.ViewMatrix();
}

void C_Camera::SetAspectRatio(const float& aspectRatio)
{
	cameraFrustum.SetHorizontalFovAndAspectRatio(cameraFrustum.HorizontalFov(), aspectRatio);
	this->aspectRatio = aspectRatio;
}

void C_Camera::SetPosition(float3 newPos)
{
	cameraFrustum.SetPos(newPos);
	//this->owner->GetTransform()->SetPosition(newPos);

}

void C_Camera::LookAt(const float3& point)
{
	reference = point;

	cameraFrustum.SetFront((reference - cameraFrustum.Pos()).Normalized());
	
	cameraFrustum.SetUp(cameraFrustum.Front().Cross(cameraFrustum.WorldRight()));

}

void C_Camera::ChangeSpeed(int multiplier)
{
	cameraSpeed = baseCameraSpeed * multiplier;
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
	
	this->owner->GetTransform()->SetPosition(cameraFrustum.Pos());
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

