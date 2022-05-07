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
#include "MathGeoLib/Math/float3x3.h"

#include "optick.h"

C_Camera::C_Camera(GameObject* parent) : Component(parent)
{
	type = ComponentType::CAMERA;
	cameraType = KOFI_PERSPECTIVE;

	//Create the frustum
	cameraFrustum = Frustum();

	//Set Default Values for the frusum
	cameraFrustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumLeftHanded);
	cameraFrustum.SetPerspective(DegToRad(43.0f), DegToRad(22.0f));
	cameraFrustum.SetHorizontalFovAndAspectRatio(DegToRad(45.0f), 1.778f);
	cameraFrustum.SetViewPlaneDistances(0.01f, 1000.0f);
	cameraFrustum.SetFrame(float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f));
	LookAt(cameraFrustum.Front());

}

C_Camera::~C_Camera()
{
}

bool C_Camera::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	owner->GetTransform()->SetGlobalTransform(cameraFrustum.WorldMatrix());

	bool ret = true;

	return ret;
}

bool C_Camera::Update(float dt)
{
	//Transform Update Camera Frustum
	//Camera Position Rotation of the camera
	if (!isEngineCamera)
	{
		C_Transform* transform = owner->GetTransform();

		cameraFrustum.SetWorldMatrix(owner->GetTransform()->GetGlobalTransform().Float3x4Part());
		//Apply rotation
		if (isFrustumCullingActive)
			FrustumCulling();

	}
	// Camera Frustum Updates Transform
	owner->GetTransform()->SetGlobalTransform(GetWorldMatrix());

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

		// PROJECTION TYPE

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		// CLEAR FLAG
		//owner->GetEngine()->GetSceneManager()->GetCurrentScene()->skybox.InspectorDraw();

		// FRUSTUM CULLING
		if (ImGui::Checkbox("Frustum culling", &isFrustumCullingActive))
		{
			ResetFrustumCulling();
		}
		// TODO: SET MAIN CAMERA TO TAG!
		if (ImGui::Checkbox("Set As Main Camera", &isMainCamera))
		{
			owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
		}

		static const char* types[]{ "Perspective", "Orthographic" };
		static int selectedItem = 0;
		if (ImGui::Combo("Combo", &selectedItem, types, IM_ARRAYSIZE(types)))
		{
			if (selectedItem == 0)
				SetProjectionType(CameraType::KOFI_PERSPECTIVE);
			if (selectedItem == 1)
				SetProjectionType(CameraType::KOFI_ORTHOGRAPHIC);


		}

		switch (cameraType)
		{
		case C_Camera::KOFI_PERSPECTIVE:
		{
			// FOV 
			float fov = GetHorizontalFov();
			if (ImGui::DragFloat("Fov", &fov, 1.0f, 0.0f, 180.f))
			{
				SetHorizontalFov(fov);
			}


			break;
		}
		}
		//PLANES
		float2 planeDistances = { cameraFrustum.NearPlaneDistance(),cameraFrustum.FarPlaneDistance() };
		ImGui::Text("Clipping Spaces");
		if (ImGui::DragFloat2("Near & Far plane distances", &(planeDistances[0])))
		{
			cameraFrustum.SetViewPlaneDistances(planeDistances.x, planeDistances.y);
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
	json["frustum_culling"] = isFrustumCullingActive;
	json["isMainCamera"] = isMainCamera;
}

void C_Camera::Load(Json& json)
{
	cameraFrustum.SetVerticalFovAndAspectRatio(json.at("vertical_fov"), 1.778f);
	cameraFrustum.SetViewPlaneDistances(json.at("near_plane_distance"), json.at("far_plane_distance"));
	isFrustumCullingActive = json.at("frustum_culling");
	isMainCamera = json.at("isMainCamera");
	if (isMainCamera)
		owner->GetEngine()->GetCamera3D()->SetGameCamera(this);
}

float4x4 C_Camera::GetViewMatrix() const
{
	return cameraFrustum.ViewMatrix();
}

float4x4 C_Camera::GetWorldMatrix() const
{
	return cameraFrustum.WorldMatrix();
}

float4x4 C_Camera::GetProjectionMatrix() const
{
	return cameraFrustum.ProjectionMatrix();
}

void C_Camera::SetAspectRatio(const float& aspectRatio)
{
	cameraFrustum.SetHorizontalFovAndAspectRatio(cameraFrustum.HorizontalFov(), aspectRatio);
}

void C_Camera::SetPosition(float3 newPos)
{
	cameraFrustum.SetPos(newPos);
}

void C_Camera::LookAt(const float3 point)
{
	reference = point;
	float3 tempFront = (reference - cameraFrustum.Pos()).Normalized();
	float3 tempRight = float3(0.0f, 1.0f, 0.0f).Cross(tempFront).Normalized();
	cameraFrustum.SetFrontUp(tempFront, tempFront.Cross(tempRight).Normalized());
}

void C_Camera::LookAt2(float3 _front, float3 _up)
{
	_front = _front.Normalized();
	_up = _up.Normalized();

	float angle = atan2(_front.z, _front.x);

	Quat r = cameraFrustum.ComputeWorldMatrix().RotatePart().ToQuat();

	float3 cross = _up.Cross(cameraFrustum.Up());
	float angleBetween = _up.AngleBetween(cameraFrustum.Up());

	r = r.RotateAxisAngle(cross, angleBetween);

	float3 currentEuler = r.ToEulerXYZ();

	float diff = currentEuler.y - angle;

	diff += 90.0f * DEGTORAD;

	r = r.RotateAxisAngle(_up, diff);

	float3 newFront = r * cameraFrustum.Front();
	cameraFrustum.SetFront(newFront);
}

void C_Camera::SetProjectionType(const CameraType& type)
{
	cameraType = type;

	if (type == CameraType::KOFI_ORTHOGRAPHIC)
	{
		hFov = cameraFrustum.HorizontalFov();
		vFov = cameraFrustum.VerticalFov();
		cameraFrustum.SetOrthographic(owner->GetEngine()->GetEditor()->viewportSize.x, owner->GetEngine()->GetEditor()->viewportSize.y);
	}
	else if (type == CameraType::KOFI_PERSPECTIVE)
	{
		cameraFrustum.SetPerspective(hFov, vFov);
		hFov = vFov = 0.0f;
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

		if (!ClipsWithBBox(componentMesh->GetLocalAABB()))
			gameObject->SetRenderGameObject(false);
		else
			gameObject->SetRenderGameObject(true);
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
			gameObject->SetRenderGameObject(true);
	}
}

void C_Camera::DrawFrustum() const
{
	float3 cornerPoints[8];
	cameraFrustum.GetCornerPoints(cornerPoints);
	//Draw Operations

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.5f);
	glBegin(GL_LINES);
	//Near plane BL-BR
	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);//Near BL
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);//Near BR
	//Near plane BL-TL
	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z); //Near BL
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z); //Near TL
	//Near plane TL - TR
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);//Near TL
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);//Near TR
	//Near plane BR - TR
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);//Near BR
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);//Near TR
	//Near plane BL - Far plane BL
	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z); //Near BL
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z); //Far BL
	//Far BL - BR
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);//Far BL
	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);//Far BR
	//Far BR - Near BR
	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z); //Far BR
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z); //Near BR
	//Far BR - TR
	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z); //Far BR
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z); //Far TR
	//Far TR - TL
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z); //Far TR
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z); //Far TL
	//Far TL - Near TL
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z); //Far TL
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z); //Near TL
	//Far TL - BL
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z); // Far TL
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z); //Far BL
	//Far TR - Near TR
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z); //Far TR
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z); //Near TR
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

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

