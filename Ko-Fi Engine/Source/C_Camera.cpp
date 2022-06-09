#include "C_Camera.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"
#include "M_Camera3D.h"
#include "M_Editor.h"
#include "M_Renderer3D.h"

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
	typeIndex = typeid(*this);

	cameraType = KOFI_PERSPECTIVE;

	//Create the frustum
	cameraFrustum = Frustum();

	//Set Default Values for the frusum
	cameraFrustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	cameraFrustum.SetPerspective(DegToRad(44.0f), DegToRad(72.57f));
	cameraFrustum.SetHorizontalFovAndAspectRatio(DegToRad(44.0f), 1.778);
	cameraFrustum.SetViewPlaneDistances(0.3f, 1000.0f);
	cameraFrustum.SetFrame(parent->GetTransform()->GetPosition(), float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f));
	LookAt(cameraFrustum.Front());
	SetIsSphereCullingActive(false);
	SetIsFrustumActive(false);
}

C_Camera::~C_Camera()
{
	CleanUp();
}

bool C_Camera::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	owner->GetTransform()->SetGlobalTransform(cameraFrustum.WorldMatrix());

	cameraFrustum.SetViewPlaneDistances(GetNearPlaneDistance(), GetFarPlaneDistance());
	tempTransform = owner->GetTransform()->GetGlobalTransform();

	bool ret = true;

	return ret;
}

bool C_Camera::PreUpdate()
{
	return true;
}

bool C_Camera::Update(float dt)
{
	OPTICK_EVENT();

	//Transform Update Camera Frustum
	//Camera Position Rotation of the camera

	// SET CAMERA FRUSTUM, OBJECT TRANSFORM
	owner->GetTransform()->SetGlobalTransform(GetWorldMatrix());
	if (!tempTransform.Equals(owner->GetTransform()->GetGlobalTransform()))
	{
		cameraFrustum.SetWorldMatrix(owner->GetTransform()->GetGlobalTransform().Float3x4Part());
		tempTransform = owner->GetTransform()->GetGlobalTransform();
	}

	
	if (!isEngineCamera && owner->GetEngine()->GetCamera3D()->currentCamera == this)
	{
		ApplyCullings();
	}
	
	return true;
}

bool C_Camera::PostUpdate(float dt)
{
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
		if (DrawDeleteButton(owner, this))
			return true;

		// CLEAR FLAG
		//owner->GetEngine()->GetSceneManager()->GetCurrentScene()->skybox.InspectorDraw();

		// FRUSTUM CULLING
		if (ImGui::Checkbox("Sphere culling", &isSphereCullingActive)) {
			owner->GetEngine()->GetRenderer()->ResetFrustumCulling();
			ApplyCullings();
		}
		// FRUSTUM CULLING
		if (ImGui::Checkbox("Frustum culling", &isFrustumCullingActive))
		{
			owner->GetEngine()->GetRenderer()->ResetFrustumCulling();
			ApplyCullings();

		}
		// SPHERE CULLING RADIUS
		ImGui::DragInt("SphereCulling Radius", &sCullingRadius, 1.0f, 1.0f, 180.f);
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
			if (ImGui::DragFloat("Fov", &fov, 1.0f, 1.0f, 180.f))
			{
				SetHorizontalFov(fov);
			}

			break;
		}
		case C_Camera::KOFI_ORTHOGRAPHIC:
		{

			float orthoWidth = GetOrthoWidth();
			if (ImGui::DragFloat("orthoWidth", &orthoWidth, 1.0f, 1.0f, 10000.0f))
			{
				SetOrthoWidth(orthoWidth);
			}
			float orthoHeight = GetOrthoHeight();
			if (ImGui::DragFloat("orthoHeight", &orthoHeight, 1.0f, 1.0f, 10000.0f))
			{
				SetOrthoHeight(orthoHeight);
			}
			if ((ImGui::Button("Apply Size Changes")))
			{
				ApplyOrthoWidthAndHeight();
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
			sCullingRadius = (planeDistances.y - planeDistances.x) / 2.0f;
		}
		ImGui::Separator();
		bool drawSkybox = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->drawSkybox;
		if (ImGui::Checkbox("Draw Skybox", &drawSkybox))
			owner->GetEngine()->GetSceneManager()->GetCurrentScene()->drawSkybox = drawSkybox;
	}
	else
		DrawDeleteButton(owner, this);


	return ret;
}

void C_Camera::Save(Json& json) const
{
	json["type"] = (int)type;
	json["vertical_fov"] = cameraFrustum.VerticalFov();
	json["near_plane_distance"] = cameraFrustum.NearPlaneDistance();
	json["far_plane_distance"] = cameraFrustum.FarPlaneDistance();
	json["sphere_culling"] = isSphereCullingActive;
	json["frustum_culling"] = isFrustumCullingActive;
	json["sCullingRadius"] = sCullingRadius;
	json["isMainCamera"] = isMainCamera;
	json["cameraType"] = (int)cameraType;
	json["orthoWidth"] = orthoWidth;
	json["orthoHeight"] = orthoHeight;
}

void C_Camera::Load(Json& json)
{
	cameraFrustum.SetVerticalFovAndAspectRatio(json.at("vertical_fov"), 1.778f);
	cameraFrustum.SetViewPlaneDistances(json.at("near_plane_distance"), json.at("far_plane_distance"));
	sCullingRadius = (GetFarPlaneDistance() - GetNearPlaneDistance()) / 2.0f;
	if (json.contains("sphere_culling"))
	{
		isSphereCullingActive = json.at("sphere_culling").get<bool>();
	}
	isFrustumCullingActive = json.at("frustum_culling");
	isMainCamera = json.at("isMainCamera");
	if (json.contains("cameraType"))
	{
		cameraType = (CameraType)json.at("cameraType");
	}
	if (json.contains("orthoWidth"))
	{
		orthoWidth = json.at("orthoWidth");
		orthoHeight = json.at("orthoHeight");
		if (cameraType == CameraType::KOFI_ORTHOGRAPHIC)
		{
			SetProjectionType(cameraType);
			ApplyOrthoWidthAndHeight();
		}
	}
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
	if(cameraType == CameraType::KOFI_PERSPECTIVE)
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
		float w = owner->GetEngine()->GetEditor()->lastViewportSize.x;
		float h = owner->GetEngine()->GetEditor()->lastViewportSize.y;
		cameraFrustum.SetOrthographic(w, h);
		
	}
	else if (type == CameraType::KOFI_PERSPECTIVE)
	{
		cameraFrustum.SetPerspective(hFov, vFov);
		hFov = vFov = 0.0f;
	}
}

void C_Camera::ApplyOrthoWidthAndHeight()
{
	cameraFrustum.SetOrthographic(orthoWidth, orthoHeight);
}

void C_Camera::SphereCulling()
{
	OPTICK_EVENT();
	//1st culling iteration where we check the distance of the game object to a middle point between the near and far plane,
	//If object is further from the sphere radius, we discard them.
	std::vector<GameObject*> gameObjects = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList;

	for (std::vector<GameObject*>::iterator go = gameObjects.begin(); go != gameObjects.end(); go++)
	{
		GameObject* gameObject = (*go);
		C_Mesh* cMesh = gameObject->GetComponent<C_Mesh>();
		if (!cMesh || gameObject == owner)
			continue;
		float3 middlePoint = this->cameraFrustum.CenterPoint(); //no need to recalculate
		float3 closest = cMesh->GetGlobalAABB().ClosestPoint(middlePoint);
		float distance = middlePoint.DistanceSq(closest);
		if (distance > (sCullingRadius * sCullingRadius))
		{
			owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.erase(gameObject);
			gameObject->isCulled = true;
			//owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.erase(gameObject);
		}
		else {
			owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.insert(gameObject);
			gameObject->isCulled = false;
			//owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.insert(gameObject);
		}

	}
}

void C_Camera::DrawSphereCulling() const
{
	glPushMatrix();
	float4x4 wM = float4x4::identity;
	wM.SetFloat3x4Part(this->cameraFrustum.WorldMatrix());
	glMultMatrixf(wM.Transposed().ptr());
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	gluSphere(gluNewQuadric(), sCullingRadius, 20, 20);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

void C_Camera::FrustumCulling()
{
	OPTICK_EVENT();

	std::unordered_set<GameObject*>::iterator it;

	for (it = owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.begin(); it != owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.end(); it++)
	{
		GameObject* go = (*it);
	/*	if (!gameObject->GetRenderGameObject())
			continue;*/
		C_Mesh* componentMesh = go->GetComponent<C_Mesh>();

		if (componentMesh == nullptr || go == owner)
			continue;

		if (owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.contains(go)) {
			if (!ClipsWithBBox(componentMesh->GetGlobalAABB()))
			{
				if (owner->GetEngine()->GetRenderer()->enableOcclusionCulling) {
					owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceOrdered.erase(go);
				}
				owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.erase(go);
				go->isCulled = true;
			}
		}
		else {
			if (ClipsWithBBox(componentMesh->GetGlobalAABB()))
			{
				if (owner->GetEngine()->GetRenderer()->enableOcclusionCulling) {
					owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceOrdered.insert(go);
				}
				owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.insert(go);
				go->isCulled = false;
			}
		}
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

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(cameraFrustum.Edge(i).a.x, cameraFrustum.Edge(i).a.y, cameraFrustum.Edge(i).a.z);
		glVertex3f(cameraFrustum.Edge(i).b.x, cameraFrustum.Edge(i).b.y, cameraFrustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

}

void C_Camera::ApplyCullings()
{
	//Apply rotation
	if (isSphereCullingActive)
		SphereCulling();
	else {
		std::vector<GameObject*> gameObjects = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->gameObjectList;

		for (std::vector<GameObject*>::iterator go = gameObjects.begin(); go != gameObjects.end(); go++)
		{
			GameObject* gameObject = (*go);
			owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.insert(gameObject);
			gameObject->isCulled = false;
		}
	}
	if (isFrustumCullingActive)
		FrustumCulling();
	else {
		std::unordered_set<GameObject*>::iterator it;

		for (it = owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.begin(); it != owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceSphere.end(); it++)
		{
			owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistance.insert(*it);
			if (owner->GetEngine()->GetRenderer()->enableOcclusionCulling) {
				owner->GetEngine()->GetRenderer()->gameObejctsToRenderDistanceOrdered.insert(*it);
			}
			(*it)->isCulled = false;
		}
	}
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

void C_Camera::SetSCullingRadius(float radius)
{
	this->sCullingRadius = radius;
}

