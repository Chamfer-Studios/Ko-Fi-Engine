#include "Globals.h"
#include "Camera3D.h"
#include "Engine.h"
#include "Input.h"
#include "SceneManager.h"
#include "Editor.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Renderer3D.h"

#include "SDL.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "MathGeoLib/Geometry/LineSegment.h"

Camera3D::Camera3D(KoFiEngine* engine) : Module()
{
	name = "Camera";
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
	this->engine = engine;
}

Camera3D::~Camera3D()
{
}

bool Camera3D::Start()
{
	CONSOLE_LOG("Setting up the camera");
	appLog->AddLog("Setting up the camera\n");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool Camera3D::CleanUp()
{
	CONSOLE_LOG("Cleaning camera");
	appLog->AddLog("Cleaning camera\n");

	return true;
}

// -----------------------------------------------------------------
bool Camera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	vec3 newPos(0, 0, 0);
	float speed = 3.0f * dt;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

	if (engine->GetInput()->GetMouseZ() > 0) newPos -= Z * speed * 3;
	if (engine->GetInput()->GetMouseZ() < 0) newPos += Z * speed * 3;

	if (engine->GetInput()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (engine->GetInput()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	vec3 spot(0, 0, 0); // Spot where the current selected game object is located.
	if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
	{
		spot.x = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().x;
		spot.y = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().y;
		spot.z = (engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID)->GetTransform())->GetPosition().z;
	}

	if (engine->GetInput()->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		Look(vec3(5,5,5), spot, true);
	}

	position += newPos;
	reference += newPos;

	// Mouse motion ----------------

	if (engine->GetInput()->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT ||
		(engine->GetInput()->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && engine->GetInput()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT))
	{
		int dx = -engine->GetInput()->GetMouseXMotion();
		int dy = -engine->GetInput()->GetMouseYMotion();

		float Sensitivity = 0.25f;

		position -= reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		position = reference + Z * length(position);

		if (engine->GetInput()->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
			Look(this->position, spot, true);
		}
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return true;
}

// -----------------------------------------------------------------
void Camera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	this->position = Position;
	this->reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->reference = this->position;
		this->position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::LookAt(const vec3& Spot)
{
	reference = Spot;

	Z = normalize(position - reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void Camera3D::Move(const vec3& Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* Camera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void Camera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void Camera3D::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void Camera3D::OnGui()
{
	if (ImGui::CollapsingHeader("Editor Camera"))
	{
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
