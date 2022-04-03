#include "Globals.h"
#include "Camera3D.h"
#include "Engine.h"
#include "Input.h"
#include "SceneManager.h"
#include "Editor.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentScript.h"
#include "Renderer3D.h"
#include "PanelViewport.h"
#include "Scripting.h"

#include "SDL.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"

Camera3D::Camera3D(KoFiEngine* engine) : Module()
{
	name = "Camera";
	this->engine = engine;

}

Camera3D::~Camera3D()
{
}

bool Camera3D::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool Camera3D::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");

	engineCameraObject = new GameObject(0, engine, "");
	engineCamera = new ComponentCamera(engineCameraObject, true);
	engineCamera->isEngineCamera = true;
	
	currentCamera = engineCamera;
	currentCamera->right = float3(1.0f, 0.0f, 0.0f);
	currentCamera->up = float3(-0.000605105015f, 0.804563046f, 0.593866348f);
	currentCamera->front = float3(-0.000820143265f, -0.593866587f, 0.804562271f);

	currentCamera->position = float3(-3.65002513f, 153.790665f, -131.349442f);
	currentCamera->reference = float3(0.0f, 0.0f, 0.0f);

	currentCamera->CalculateViewMatrix();

	currentCamera->LookAt(float3::zero);

	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool Camera3D::Update(float dt)
{
	if (engine->GetEditor()->GetPanel<PanelViewport>())
		if (!engine->GetEditor()->GetPanel<PanelViewport>()->IsWindowFocused())
			return true;

	if (currentCamera->isEngineCamera)
	{
		CheckMouseMotion(dt);

	}

	return true;
}

// -----------------------------------------------------------------
bool Camera3D::CleanUp()
{
	CONSOLE_LOG("Cleaning camera");
	appLog->AddLog("Cleaning camera\n");
	//RELEASE(currentCamera);
	//RELEASE(engineCamera);
	//RELEASE(gameCamera);
	return true;
}

// Method to receive and manage events
void Camera3D::OnNotify(const Event& event)
{
	// Manage events
}

void Camera3D::OnGui()
{
	if (ImGui::CollapsingHeader("Editor Camera"))
	{
		if (ImGui::DragFloat("Vertical fov", &currentCamera->verticalFOV))
		{
			currentCamera->projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Near plane distance", &currentCamera->nearPlaneDistance))
		{
			currentCamera->projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Far plane distance", &currentCamera->farPlaneDistance))
		{
			currentCamera->projectionIsDirty = true;
		}
	}
}

void Camera3D::CheckInput(float dt)
{
	float3 newPos(0, 0, 0);
	float speed = currentCamera->cameraSpeed * dt;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed *= 4.f;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;

	// Focus --> NEEDS TO BE FIXED... SOME (MESH) FUNCTIONS DEPEND ON A PRIMITIVE LIBRARY WE STILL DON'T HAVE IMPLEMENTED.
	if (engine->GetInput()->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		// TO DO: Manage current object selection by the game object itself! Not by its index...
		if (/*engine->GetEditor()->gameobjectSelected != nullptr <-- Should be this way*/
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
		{
			// If we change the previous TO DO, this will be no longer needed...
			GameObject* gameObjectSelected =
				engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);

			if (ComponentMesh* mesh = /*App->editor->gameobjectSelected->GetComponent<ComponentMesh>()*/
				gameObjectSelected->GetComponent<ComponentMesh>())
			{
				const float3 meshCenter = mesh->GetCenterPointInWorldCoords(); // FIX THIS FUNCTION
				currentCamera->LookAt(meshCenter);
				const float meshRadius = mesh->GetSphereRadius(); // FIX THIS FUNCTION
				const float currentDistance = meshCenter.Distance(currentCamera->position);
				const float desiredDistance = (meshRadius * 2) / atan(currentCamera->cameraFrustum.horizontalFov);
				currentCamera->position = currentCamera->position + currentCamera->front * (currentDistance - desiredDistance);
			}
			else
			{
				ComponentTransform* transform = gameObjectSelected->GetTransform();
				if (transform != nullptr)
					currentCamera->LookAt(gameObjectSelected->GetTransform()->GetPosition());
			}
		}
	}

	vec3 spot(0, 0, 0); // Spot where the current selected game object is located.
	if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
	{
		ComponentTransform* transform = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform();
		if (transform != nullptr) {
			spot.x = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().x;
			spot.y = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().y;
			spot.z = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().z;
		}
	}
	if (engine->GetInput()->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		currentCamera->LookAt(float3(5, 5, 5));
	}

	if (engine->GetInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos += currentCamera->front * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos -= currentCamera->front * speed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos += currentCamera->right * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos -= currentCamera->right * speed;

	if (engine->GetInput()->GetMouseZ() > 0) newPos += currentCamera->front * speed * 2;
	if (engine->GetInput()->GetMouseZ() < 0) newPos -= currentCamera->front * speed * 2;

	currentCamera->position += newPos; // MODULE CAMERA REVISION CHECKPOINT --> CHECK AND FIX ERRORS FIRST!
}

void Camera3D::CheckMouseMotion(float dt)
{

	// Mouse motion ----------------

	bool hasRotated = false;

	if (engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		CheckInput(dt);
		int dx = -engine->GetInput()->GetMouseXMotion();
		int dy = -engine->GetInput()->GetMouseYMotion();

		if (engine->GetInput()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
			if (/*engine->GetEditor()->gameobjectSelected != nullptr*/
				engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
			{
				const float newDeltaX = (float)dx * currentCamera->cameraSensitivity;
				const float newDeltaY = (float)dy * currentCamera->cameraSensitivity;

				currentCamera->reference = /*engine->GetEditor()->gameobjectSelected->transform->GetPosition()*/
					engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()
						->panelGameObjectInfo.selectedGameObjectID)->GetComponent<ComponentTransform>()->GetPosition();
				Quat orbitMat = Quat::RotateY(newDeltaX * .1f);

				if (abs(currentCamera->up.y) < 0.3f) // Avoid gimball lock on up & down apex
				{
					if (currentCamera->position.y > currentCamera->reference.y && newDeltaY < 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(currentCamera->right, newDeltaY * .1f);
					if (currentCamera->position.y < currentCamera->reference.y && newDeltaY > 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(currentCamera->right, newDeltaY * .1f);
				}
				else
				{
					orbitMat = orbitMat * math::Quat::RotateAxisAngle(currentCamera->right, newDeltaY * .1f);
				}

				currentCamera->position = orbitMat * (currentCamera->position - currentCamera->reference) + currentCamera->reference;

				currentCamera->CalculateViewMatrix();
				currentCamera->LookAt(currentCamera->reference);
			}
		}
		else
		{
			if (dx != 0)
			{
				const float newDeltaX = (float)dx * currentCamera->cameraSensitivity;
				float deltaX = newDeltaX + 0.95f * (currentCamera->lastDeltaX - newDeltaX); //lerp for smooth rotation acceleration to avoid jittering
				currentCamera->lastDeltaX = deltaX;
				Quat rotateY = Quat::RotateY(currentCamera->up.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
				currentCamera->up = rotateY * currentCamera->up;
				currentCamera->front = rotateY * currentCamera->front;
				currentCamera->CalculateViewMatrix();
				hasRotated = true;
			}

			if (dy != 0)
			{
				const float newDeltaY = (float)dy * currentCamera->cameraSensitivity;
				float deltaY = newDeltaY + 0.95f * (currentCamera->lastDeltaY - newDeltaY); //lerp for smooth rotation acceleration to avoid jittering
				currentCamera->lastDeltaY = deltaY;
				Quat rotateX = Quat::RotateAxisAngle(currentCamera->right, -deltaY * .1f);
				currentCamera->up = rotateX * currentCamera->up;
				currentCamera->front = rotateX * currentCamera->front;
				currentCamera->CalculateViewMatrix();
				hasRotated = true;
			}
		}
	}

	!hasRotated ? currentCamera->lastDeltaX = currentCamera->lastDeltaY = 0.f : 0.f;

	currentCamera->CalculateViewMatrix();
}

void Camera3D::OnPlay()
{
	if (gameCamera == nullptr)
	{
		GameObject* go = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("MainCamera");
		ComponentCamera* cCamera = go->CreateComponent<ComponentCamera>();
		cCamera->isMainCamera = true;
		gameCamera = cCamera;

	}
		
	currentCamera = gameCamera;
}

void Camera3D::OnStop()
{
	currentCamera = engineCamera;
}

bool Camera3D::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool Camera3D::LoadConfiguration(Json& configModule)
{
	return true;
}

bool Camera3D::InspectorDraw()
{
	return true;
}

void Camera3D::SetGameCamera(ComponentCamera* gameCamera)
{
	this->gameCamera = gameCamera;
}

void Camera3D::OnClick(SDL_Event event)
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
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = hit->GetUID();
		}
		else {
			engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID = -1;
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

//void Camera3D::OnSave(JSONWriter& writer) const
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
//void Camera3D::OnLoad(const JSONReader& reader)
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

GameObject* Camera3D::MousePicking(const bool& isRightButton)
{
	float normalX = engine->GetEditor()->mouseScenePosition.x / engine->GetEditor()->lastViewportSize.x;
	float normalY = engine->GetEditor()->mouseScenePosition.y / engine->GetEditor()->lastViewportSize.y;

	normalX = (normalX - 0.5f) * 2.0f;
	normalY = -(normalY - 0.5f) * 2.0f;
	/*CONSOLE_LOG("x: %f", normalX);
	CONSOLE_LOG("y: %f", normalY);*/

	LineSegment newRay = currentCamera->cameraFrustum.UnProjectLineSegment(normalX, normalY);
	engine->GetSceneManager()->GetCurrentScene()->ray = newRay;

	std::vector<GameObject*> sceneGameObjects = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;
	std::map<float, GameObject*> hitGameObjects;

	// Find all hit GameObjects
	for (size_t i = 0; i < sceneGameObjects.size(); i++)
	{
		ComponentMesh* m = sceneGameObjects[i]->GetComponent<ComponentMesh>();
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
		rayLocal.Transform(gameObject->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted());

		ComponentMesh* cMesh = gameObject->GetComponent<ComponentMesh>();

		if (cMesh != nullptr)
		{
			Mesh* rMesh = cMesh->GetMesh();

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
				float3 intersectionPoint;
				//if (rayLocal.Intersects(triangle, &distance, &intersectionPoint)) return gameObject;

				if (rayLocal.Intersects(triangle, &distance, &intersectionPoint))
				{
					if (!isRightButton)
						return gameObject;

					for (GameObject* go : sceneGameObjects)
					{
						//if (gameObject != go && engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID == go->GetUID())
						{
							ComponentScript* script = go->GetComponent<ComponentScript>();
							if (script != nullptr)
							{
								if (script->path.substr(script->path.find_last_of('/') + 1) == "Player.lua" && (bool)script->handler->lua["IsSelected"]())
								{
									intersectionPoint.x *= gameObject->GetTransform()->GetScale().x;
									intersectionPoint.y *= gameObject->GetTransform()->GetScale().y;
									intersectionPoint.z *= gameObject->GetTransform()->GetScale().z;
									script->handler->lua["destination"] = intersectionPoint;
								}
							}
						}
					}
				}
			}
		}
	}

	return nullptr;
}