#include "Globals.h"
#include "M_Camera3D.h"
#include "Engine.h"
#include "M_Input.h"
#include "M_SceneManager.h"
#include "M_Editor.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Script.h"
#include "M_Renderer3D.h"
#include "PanelViewport.h"
#include "Scripting.h"
#include "GameObject.h"

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
	engineCameraObject = new GameObject(0, engine, "");
	engineCamera = new C_Camera(engineCameraObject);
	engineCamera->SetIsEngineCamera(true);

	engineCamera->SetReference(float3(0.0f, 0.0f, 0.0f));
	engineCamera->SetFarPlaneDistance(4000.0f);
	engineCamera->LookAt(engineCamera->GetFront());
	engineCamera->SetIsFrustumActive(true);

	currentCamera = engineCamera;
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


	// Update Engine Transform
	engineCamera->Update(dt); // Update First**
	engineCamera->owner->GetTransform()->Update(dt);
	engineCamera->owner->GetTransform()->PostUpdate(dt);

	if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused())
		return true;

	if (currentCamera->IsEngineCamera() && engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		CheckInput(dt);
		MouseZoom(dt);
		MouseRotation(dt);
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
	float speed = engineCamera->GetCameraSpeed() * dt;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed *= 5.0f;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += engineCamera->GetFront() * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= engineCamera->GetFront() * speed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= engineCamera->GetRight() * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += engineCamera->GetRight() * speed;

	engineCamera->SetPosition(engineCamera->GetPosition() + newPos);
}

void M_Camera3D::MouseZoom(float dt)
{
	float3 newPos = float3(0.f, 0.f, 0.f);

	// Mouse Middle Button
	if (engine->GetInput()->GetMouseZ() > 0) newPos += engineCamera->GetFront() * engineCamera->GetCameraSpeed();
	if (engine->GetInput()->GetMouseZ() < 0) newPos -= engineCamera->GetFront() * engineCamera->GetCameraSpeed();

	engineCamera->SetPosition(engineCamera->GetPosition() + newPos);
}

void M_Camera3D::MouseRotation(float dt)
{
	// TODO: MOUSE MOTION
	bool hasRotated = false;

	int dx = -engine->GetInput()->GetMouseXMotion();
	int dy = -engine->GetInput()->GetMouseYMotion();

	const float newDeltaX = (float)dx * engineCamera->GetCameraSensitivity();
	const float newDeltaY = (float)dy * engineCamera->GetCameraSensitivity();

	//if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1) // If Object is Selected
	//{

	//	// Orbit Around The Object
	//	engineCamera->SetReference(engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetComponent<C_Transform>()->GetPosition());

	//	Quat orbitMat = Quat::RotateY(newDeltaX * .1f);

	//	if (abs(engineCamera->GetUp().y) < 0.3f) // Avoid gimball lock on up & down apex
	//	{
	//		if (engineCamera->GetPosition().y > engineCamera->GetReference().y && newDeltaY < 0.f)
	//			orbitMat = orbitMat * math::Quat::RotateAxisAngle(engineCamera->GetRight(), newDeltaY * .1f);
	//		if (engineCamera->GetPosition().y < engineCamera->GetReference().y && newDeltaY > 0.f)
	//			orbitMat = orbitMat * math::Quat::RotateAxisAngle(engineCamera->GetRight(), newDeltaY * .1f);
	//	}
	//	else
	//	{
	//		orbitMat = orbitMat * math::Quat::RotateAxisAngle(engineCamera->GetRight(), newDeltaY * .1f);
	//	}

	//	engineCamera->SetPosition(orbitMat * (currentCamera->GetPosition() - currentCamera->GetReference()) + currentCamera->GetReference());
	//	engineCamera->LookAt(engineCamera->GetReference());
	//}


	if (dx != 0)
	{
		const float newDeltaX = (float)dx * engineCamera->GetCameraSensitivity();
		float deltaX = newDeltaX + 0.95f * (engineCamera->GetLastDeltaX() - newDeltaX); //lerp for smooth rotation acceleration to avoid jittering
		engineCamera->SetLastDeltaX(deltaX);
		Quat rotateY = Quat::RotateY(engineCamera->GetUp().y >= 0.f ? deltaX * .1f : -deltaX * .1f);
		//engineCamera->SetUp(rotateY * engineCamera->GetUp());
		engineCamera->SetFront(rotateY * engineCamera->GetFront());
		hasRotated = true;
	}

	if (dy != 0)
	{
		const float newDeltaY = (float)dy * engineCamera->GetCameraSensitivity();
		float deltaY = newDeltaY + 0.95f * (engineCamera->GetLastDeltaY() - newDeltaY); //lerp for smooth rotation acceleration to avoid jittering
		engineCamera->SetLastDeltaY(deltaY);
		Quat rotateX = Quat::RotateAxisAngle(engineCamera->GetRight(), -deltaY * .1f);
		//engineCamera->SetUp(rotateX * engineCamera->GetUp());
		engineCamera->SetFront(rotateX * engineCamera->GetFront());
		hasRotated = true;
	}



	if (!hasRotated)
	{
		engineCamera->SetLastDeltaX(0.0f);
		engineCamera->SetLastDeltaY(0.0f);
	}

}



void M_Camera3D::OnPlay()
{
	if (gameCamera == nullptr)
	{
		GameObject* go = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("MainCamera");
		C_Camera* cCamera = go->CreateComponent<C_Camera>();
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
	if (ImGui::CollapsingHeader("Engine Camera##"))
	{
		int newSpeedMultiplier = engineCamera->GetSpeedMultiplier();
		if (ImGui::SliderInt("Camera Speed", &newSpeedMultiplier, 1.0f, 5.0f))
		{
			engineCamera->ChangeSpeed(newSpeedMultiplier);
		}
		//Frustum Active
		bool frustumActive = engineCamera->GetIsFrustumActive();
		if (ImGui::Checkbox("Frustum culling", &frustumActive))
		{
			engineCamera->SetIsFrustumActive(frustumActive);
		}
		float newHorizontallFov = currentCamera->GetHorizontalFov();
		if (ImGui::DragFloat("Fov", &newHorizontallFov, 0.5f, 1.0f, 179.f))
		{
			currentCamera->SetHorizontalFov(newHorizontallFov);
		}

		float2 planeDistances = { currentCamera->GetNearPlaneDistance(),currentCamera->GetFarPlaneDistance() };
		if (ImGui::DragFloat2("Near plane distance", &(planeDistances[0])))
		{
			currentCamera->SetViewPlaneDistances(planeDistances.x, planeDistances.y);
		}

			ImGui::Text("x: %f y: %f z: %f", engineCamera->GetPosition().x, engineCamera->GetPosition().y, engineCamera->GetPosition().z);

		// Position ImGui
		float3 newPosition = engineCamera->owner->GetTransform()->GetPosition();
		if (ImGui::DragFloat3("Location##", &(newPosition[0]), 0.5f))
		{
			engineCamera->SetPosition(newPosition);
		}

		// Rotation ImGui
		float3 newRotationEuler = engineCamera->owner->GetTransform()->GetRotationEuler();
		newRotationEuler = RadToDeg(newRotationEuler);
		if (ImGui::DragFloat3("Rotation##", &(newRotationEuler[0]), 0.045f))
		{
			newRotationEuler = DegToRad(newRotationEuler);
			engineCamera->owner->GetTransform()->SetRotationEuler(newRotationEuler);
		}

	}
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
			else
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
	/*CONSOLE_LOG("x: %f", normalX);
	CONSOLE_LOG("y: %f", normalY);*/

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
					//float tmp = lastMouseClick.y;
					//lastMouseClick.y = lastMouseClick.z;
					//lastMouseClick.z = tmp;
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


