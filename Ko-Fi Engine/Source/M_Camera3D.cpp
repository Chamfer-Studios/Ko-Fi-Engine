#include "Globals.h"
#include "M_Camera3D.h"

// Modules
#include "Engine.h"
#include "M_Input.h"
#include "M_SceneManager.h"
#include "M_Editor.h"
#include "M_Renderer3D.h"

// GameObjects
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Script.h"

#include "PanelViewport.h"
#include "Scripting.h"

#include "SDL.h"
#include "Log.h"
#include "ImGuiAppLog.h"

#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"
#include "MathGeoLib/Math/float4.h"

#include "optick.h"

M_Camera3D::M_Camera3D(KoFiEngine* engine) : Module()
{
	name = "Camera";
	this->engine = engine;

	//Set Default Values
	cameraSensitivity = 5.0f;
	cameraSpeed = 1.f;
	baseCameraSpeed = 1.f;
	speedMultiplier = 1;

	// Initialize Engine Camera
	engineCameraObject = new GameObject(0, engine, "");
	engineCamera = new C_Camera(engineCameraObject);
	engineCamera->SetIsEngineCamera(true);
	
	engineCamera->SetReference(float3(0.0f, 0.0f, 0.0f));
	engineCamera->SetViewPlaneDistances(.3f,20000.0f);
	engineCamera->LookAt(engineCamera->GetFront());
	engineCamera->SetIsSphereCullingActive(true);
	engineCamera->SetIsFrustumActive(true);
	currentCamera = engineCamera;
	engineCamera->SetSCullingRadius(20000.0f / 2);
}

M_Camera3D::~M_Camera3D()
{
}

bool M_Camera3D::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool M_Camera3D::Start()
{
	bool ret = true;

	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	engineCamera->SetPosition(float3(0.0f, 0.0f, -100.f));

	return ret;
}

// -----------------------------------------------------------------
bool M_Camera3D::Update(float dt)
{
	OPTICK_EVENT();

	FocusTarget();

	//if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused() && isMoving == false)
	//	return true;

	if (currentCamera->IsEngineCamera() && engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		CheckInput(dt);
		MouseZoom(dt);
		MouseRotation(dt);
		lastDeltaX = 0.0f;
		lastDeltaY = 0.0f;
		isMoving = true;
	}
	else {
		isMoving = false;
		cameraSpeed = baseCameraSpeed;
	}
	
	if (currentCamera == engineCamera) {
		engineCamera->ApplyCullings();
	}
	return true;
}

// -----------------------------------------------------------------
bool M_Camera3D::CleanUp()
{
	CONSOLE_LOG("Cleaning camera");
	appLog->AddLog("Cleaning camera\n");
	//RELEASE(currentCamera);
	//RELEASE(engineCamera);
	//RELEASE(gameCamera);
	return true;
}

// Method to receive and manage events
void M_Camera3D::OnNotify(const Event& event)
{
	// Manage events
}

void M_Camera3D::OnGui()
{
	// NOT BEING CALLED, USE INSPECTORDRAW

	//if (ImGui::CollapsingHeader("Module Camera"))
	//{
	//	float newHorizontallFov = RadToDeg(currentCamera->GetHorizontalFov());
	//	if (ImGui::DragFloat("Fov", &newHorizontallFov))
	//	{
	//		currentCamera->SetHorizontalFov(newHorizontallFov);
	//	}
	//	float2 planeDistances = { currentCamera->GetNearPlaneDistance(),currentCamera->GetFarPlaneDistance() };
	//	if (ImGui::DragFloat("Near plane distance", &(planeDistances[0])))
	//	{
	//		currentCamera->SetViewPlaneDistances(planeDistances.x, planeDistances.y);
	//	}

	//}


}

void M_Camera3D::CheckInput(float dt)
{
	float3 newPos(0, 0, 0);
	if (cameraSpeed >= maxSpeed)
		cameraSpeed = maxSpeed;

	float speed = cameraSpeed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed *= 2.0f;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += engineCamera->GetFront() * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= engineCamera->GetFront() * speed ;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= engineCamera->GetRight() * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += engineCamera->GetRight() * speed;
	
	engineCamera->SetPosition(engineCamera->GetPosition() + newPos);
	cameraSpeed =cameraSpeed + (cameraSpeed* dt);
}

void M_Camera3D::MouseZoom(float dt)
{
	float3 newPos = float3(0.f, 0.f, 0.f);

	// Mouse Middle Button
	if (engine->GetInput()->GetMouseZ() > 0) newPos += engineCamera->GetFront() * cameraSpeed;
	if (engine->GetInput()->GetMouseZ() < 0) newPos -= engineCamera->GetFront() * cameraSpeed;

	engineCamera->SetPosition(engineCamera->GetPosition() + newPos);
}

// A function that takes the delta mouse motion as x and y inputs, creates a quaternion representing a rotation based on the mouse motion, and applies that rotation to the camera's orientation.
void M_Camera3D::MouseRotation(float dt)
{
	int xMotion = -engine->GetInput()->GetMouseXMotion(); // (-) Because is a Right Handed Camera
	int yMotion = -engine->GetInput()->GetMouseYMotion(); // (-) Because is a Right Handed Camera
	if (xMotion != 0) {
		const float newDeltaX = (float)xMotion * cameraSensitivity;
		float deltaX = newDeltaX + 0.95f * (lastDeltaX - newDeltaX);
		lastDeltaX = deltaX;
		int xDir = engineCamera->GetUp().y > 0 ? 1 : -1;
		float angle = xDir * deltaX * dt;
		
		Quat rot = Quat::RotateY(angle);
		engineCamera->SetFrontAndUp(rot * engineCamera->GetFront(), rot * engineCamera->GetUp());


	}

	if (yMotion != 0) {
		const float newDeltaY = (float)yMotion * cameraSensitivity;
		float deltaY = newDeltaY + 0.95f * (lastDeltaY - newDeltaY);
		lastDeltaY = deltaY;
		float angle = deltaY * dt;
		Quat rot = Quat::RotateAxisAngle(currentCamera->GetRight(),angle);
		engineCamera->SetFrontAndUp( rot* engineCamera->GetFront(), rot * engineCamera->GetUp());
	}

}

void M_Camera3D::FocusTarget()
{
	if((engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size() == 0)) return;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[0] != -1)
		{
			GameObject* selectedGameObject = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[0]);
			C_Mesh* mesh = selectedGameObject->GetComponent<C_Mesh>();
			if (mesh && mesh->GetMesh())
			{
				
				const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
				engineCamera->LookAt(meshCenter);
				const float meshRadius = mesh->GetSphereRadius(); // FIX THIS FUNCTION
				const float currentDistance = meshCenter.Distance(currentCamera->GetPosition());
				const float desiredDistance = (meshRadius * 2) / atan(currentCamera->GetCameraFrustum().HorizontalFov());
				currentCamera->SetPosition(currentCamera->GetPosition() + currentCamera->GetFront() * (currentDistance - desiredDistance));
			}
			else
			{
				C_Transform* transform = selectedGameObject->GetTransform();
				if (transform != nullptr)
					currentCamera->LookAt(selectedGameObject->GetTransform()->GetPosition());
			}
		}
	}
}

void M_Camera3D::OnPlay()
{
	if (gameCamera == nullptr)
	{
		GameObject* go = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("MainCamera");
		C_Camera* cCamera = (C_Camera*)go->AddComponentByType(ComponentType::CAMERA);
		cCamera->SetIsMainCamera(true);
		gameCamera = cCamera;

	}

	currentCamera = gameCamera;
}

void M_Camera3D::OnStop()
{
	currentCamera = engineCamera;
}

bool M_Camera3D::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Camera3D::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Camera3D::InspectorDraw()
{
	//if (ImGui::CollapsingHeader("Engine Camera##"))
	//{
	//	int newSpeedMultiplier = speedMultiplier;
	//	if (ImGui::SliderInt("Camera Speed", &newSpeedMultiplier, 1.0f, 5.0f))
	//	{
	//		ChangeSpeed(newSpeedMultiplier);
	//	}

	//	//	Frustum Active
	//	bool frustumActive = engineCamera->GetIsFrustumActive();
	//	if (ImGui::Checkbox("Frustum culling", &frustumActive))
	//	{
	//		engineCamera->SetIsFrustumActive(frustumActive);
	//	}

	//	static const char* types[]{ "Perspective", "Orthographic" };
	//	static int selectedItem = 0;
	//	if (ImGui::Combo("Combo", &selectedItem, types, IM_ARRAYSIZE(types)))
	//	{
	//		if (selectedItem == 0)
	//			engineCamera->SetProjectionType(C_Camera::CameraType::KOFI_PERSPECTIVE);
	//		if (selectedItem == 1)
	//			engineCamera->SetProjectionType(C_Camera::CameraType::KOFI_ORTHOGRAPHIC);


	//	}

	//}
	engineCamera->InspectorDraw(nullptr);
	return true;
}

void M_Camera3D::SetGameCamera(C_Camera* gameCamera)
{
	if (this->gameCamera != nullptr && this->gameCamera != gameCamera)this->gameCamera->SetIsMainCamera(false);

	this->gameCamera = gameCamera;
	if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING)
	{
		this->currentCamera = this->gameCamera;
	}
}

void M_Camera3D::OnClick(SDL_Event event)
{
	if (event.button.type != SDL_MOUSEBUTTONDOWN || (event.button.button != SDL_BUTTON_LEFT && event.button.button != SDL_BUTTON_RIGHT)) return;
	if (engine->GetEditor()->GetPanel<PanelViewport>())
		if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused())
			return;

	if (event.button.button == SDL_BUTTON_LEFT && engine->GetSceneManager()->GetGameState() != GameState::PLAYING && !ImGuizmo::IsOver())
	{
		GameObject* hit = engine->GetCamera3D()->MousePicking();
		if (hit != nullptr)
		{
			CONSOLE_LOG("%s", hit->GetName());
			if (engine->GetEditor()->contr == true && engine->GetSceneManager()->GetGameState() != GameState::PLAYING && !ImGuizmo::IsOver())
			{
				auto it = std::find(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.begin(), engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.end(), hit->GetUID());
				if (it != engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.end()) return;

				engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.push_back(hit->GetUID());
			}
			else if(engine->GetSceneManager()->GetGameState() != GameState::PLAYING && !ImGuizmo::IsOver())
			{
				engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.clear();
				engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.shrink_to_fit();
				engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.push_back(hit->GetUID());
			}
			//engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = hit->GetUID();
		}
		else {
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.clear();
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.shrink_to_fit();
		}
	}
	else if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			engine->GetCamera3D()->MousePicking();
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			engine->GetCamera3D()->MousePicking(true);
		}
	}
}

//void M_Camera3D::OnSave(JSONWriter& writer) const
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
//
//void M_Camera3D::OnLoad(const JSONReader& reader)
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

GameObject* M_Camera3D::MousePicking(const bool& isRightButton)
{
	float normalX = engine->GetEditor()->mouseScenePosition.x / engine->GetEditor()->lastViewportSize.x;
	float normalY = engine->GetEditor()->mouseScenePosition.y / engine->GetEditor()->lastViewportSize.y;

	normalX = (normalX - 0.5f) * 2.0f;
	normalY = -(normalY - 0.5f) * 2.0f;

	LineSegment newRay = currentCamera->GetCameraFrustum().UnProjectLineSegment(normalX, normalY);
	engine->GetSceneManager()->GetCurrentScene()->ray = newRay;

	std::vector<GameObject*> sceneGameObjects = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;
	std::map<float, GameObject*> hitGameObjects;

	// Find all hit GameObjects
	for (size_t i = 0; i < sceneGameObjects.size(); i++)
	{
		C_Mesh* m = sceneGameObjects[i]->GetComponent<C_Mesh>();
		if (m != nullptr)
		{
			bool hit = newRay.Intersects(m->GetGlobalAABB());

			if (hit)
			{
				float dNear;
				float dFar;
				hit = newRay.Intersects(m->GetGlobalAABB(), dNear, dFar);
				hitGameObjects[dNear] = sceneGameObjects[i];
			}
		}
	}

	for (std::map<float, GameObject*>::iterator it = hitGameObjects.begin(); it != hitGameObjects.end(); it++)
	{
		GameObject* gameObject = it->second;

		if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING)
		{
			if (gameObject->tag == TAG::TAG_DECORATION_FLOOR)
				continue;
		}

		LineSegment rayLocal = newRay;
		rayLocal.Transform(gameObject->GetComponent<C_Transform>()->GetGlobalTransform().Inverted());

		C_Mesh* cMesh = gameObject->GetComponent<C_Mesh>();

		if (cMesh != nullptr)
		{
			R_Mesh* rMesh = cMesh->GetMesh();

			if (rMesh == nullptr) continue;

			// Convert our float pointer to a std::vector of float3
			std::vector<float3> vertices;
			float* fvertices = rMesh->vertices;
			for (int i = 0; i < rMesh->verticesSizeBytes / sizeof(float); i += 3)
			{
				vertices.push_back(float3(fvertices[i], fvertices[i + 1], fvertices[i + 2]));
			}

			for (size_t i = 0; i < rMesh->indicesSizeBytes / sizeof(unsigned int); i += 3)
			{
				// Create every triangle
				float3 v1;
				v1.x = vertices[rMesh->indices[i]].x;
				v1.y = vertices[rMesh->indices[i]].y;
				v1.z = vertices[rMesh->indices[i]].z;

				float3 v2;
				v2.x = vertices[rMesh->indices[i + 1]].x;
				v2.y = vertices[rMesh->indices[i + 1]].y;
				v2.z = vertices[rMesh->indices[i + 1]].z;

				float3 v3;
				v3.x = vertices[rMesh->indices[i + 2]].x;
				v3.y = vertices[rMesh->indices[i + 2]].y;
				v3.z = vertices[rMesh->indices[i + 2]].z;

				const Triangle triangle(v1, v2, v3);

				float distance;
				if (rayLocal.Intersects(triangle, &distance, &lastMouseClick))
				{
					lastMouseClick.x *= gameObject->GetTransform()->GetScale().x;
					lastMouseClick.y *= gameObject->GetTransform()->GetScale().y;
					lastMouseClick.z *= gameObject->GetTransform()->GetScale().z;
					lastMouseClick.x += gameObject->GetTransform()->GetPosition().x;
					lastMouseClick.y += gameObject->GetTransform()->GetPosition().y;
					lastMouseClick.z += gameObject->GetTransform()->GetPosition().z;
					return gameObject;
				}
			}
		}
	}

	return nullptr;
}

float2 M_Camera3D::WorldToScreen(float3 position)
{
	float4 clipSpacePos = currentCamera->GetCameraFrustum().ProjectionMatrix() * (currentCamera->GetCameraFrustum().ViewMatrix() * float4(position, 1.0f));
	float3 ndcSpacePos = clipSpacePos.xyz() / clipSpacePos.w;
	float2 partial = ((ndcSpacePos.xy() + float2(1.0f)) / 2.0f);
	float2 viewport = float2(engine->GetEditor()->lastViewportSize.x, engine->GetEditor()->lastViewportSize.y);
	return float2(partial.x * viewport.x, partial.y * viewport.y) + float2(0, 0);
}

float3 M_Camera3D::GetLastMouseClick() const
{
	return lastMouseClick;
}


