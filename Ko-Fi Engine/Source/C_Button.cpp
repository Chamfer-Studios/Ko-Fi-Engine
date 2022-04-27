#include "C_Button.h"

#include "SDL.h"
#include "SDL_image.h"

// Modules
#include "Engine.h"
#include "M_UI.h"
#include "M_Input.h"
#include "M_SceneManager.h"
#include "M_Camera3d.h"

// GameObject
#include "GameObject.h"
#include "C_Canvas.h"

#include "Importer.h"
#include "PanelChooser.h"
#include "ImGuiAppLog.h"

C_Button::C_Button(GameObject* parent) : C_RenderedUI(parent)
{
	type = ComponentType::BUTTON;
}

C_Button::~C_Button()
{
	FreeTextures(BUTTON_STATE::IDLE);
	FreeTextures(BUTTON_STATE::HOVER);
	FreeTextures(BUTTON_STATE::PRESSED);
}

bool C_Button::CleanUp()
{
	FreeTextures(BUTTON_STATE::IDLE);
	FreeTextures(BUTTON_STATE::HOVER);
	FreeTextures(BUTTON_STATE::PRESSED);
	return true;
}


bool C_Button::Update(float dt)
{
	float2 mouseScreenPosition = { (float)owner->GetEngine()->GetInput()->GetMouseX(), (float)owner->GetEngine()->GetInput()->GetMouseY() };
	C_Canvas* canvas = owner->GetComponent<C_Transform2D>()->GetCanvas();
	if (canvas != nullptr) {
		float2 mouseLogicalPosition = canvas->ScreenToLogical(mouseScreenPosition);
		bool mouseWithinBounds = owner->GetComponent<C_Transform2D>()->CheckPointWithinBounds(mouseLogicalPosition);

		bool mouseLeftRepeat = owner->GetEngine()->GetInput()->GetMouseButton(1) == KEY_STATE::KEY_REPEAT;
		bool mouseLeftUp = owner->GetEngine()->GetInput()->GetMouseButton(1) == KEY_STATE::KEY_UP;

		if (mouseWithinBounds && !mouseLeftRepeat) {
			state = BUTTON_STATE::HOVER;
		}
		else if (mouseWithinBounds && mouseLeftRepeat) {
			state = BUTTON_STATE::PRESSED;
		}
		else if (!mouseWithinBounds) {
			state = BUTTON_STATE::IDLE;
		}

		if (mouseWithinBounds && mouseLeftUp) {
			appLog->AddLog("Pressed!");
		}
	}

	return true;
}

bool C_Button::PostUpdate(float dt)
{
	return true;
}

bool C_Button::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		// IDLE
		ImGui::Text("IDLE: ");
		ImGui::SameLine();
		if (idleOpenGLTexture.GetTextureId() == TEXTUREID_DEFAULT)
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(idleOpenGLTexture.GetTexturePath());
			ImGui::Image((ImTextureID)idleOpenGLTexture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("IDLETextureButton")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				SetIdleTexture(path.c_str());
			}
		}

		if (ImGui::Button("Set IDLE texture")) {
			panelChooser->OpenPanel("IDLETextureButton", "png", { "png","jpg","jpeg" });
		}

		// HOVER
		ImGui::Text("HOVER: ");
		ImGui::SameLine();
		if (hoverOpenGLTexture.GetTextureId() == TEXTUREID_DEFAULT)
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(hoverOpenGLTexture.GetTexturePath());
			ImGui::Image((ImTextureID)hoverOpenGLTexture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("HOVERTextureButton")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				SetHoverTexture(path.c_str());
			}
		}

		if (ImGui::Button("Set HOVER texture")) {
			panelChooser->OpenPanel("HOVERTextureButton", "png", { "png","jpg","jpeg" });
		}

		// PRESSED
		ImGui::Text("PRESSED: ");
		ImGui::SameLine();
		if (pressedOpenGLTexture.GetTextureId() == TEXTUREID_DEFAULT)
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(pressedOpenGLTexture.GetTexturePath());
			ImGui::Image((ImTextureID)pressedOpenGLTexture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("PRESSEDTextureButton")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				SetPressedTexture(path.c_str());
			}
		}

		if (ImGui::Button("Set PRESSED texture")) {
			panelChooser->OpenPanel("PRESSEDTextureButton", "png", { "png","jpg","jpeg" });
		}
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}

void C_Button::Save(Json& json) const
{
	json["idleTexture"] = idleOpenGLTexture.GetTexturePath();
	json["hoverTexture"] = hoverOpenGLTexture.GetTexturePath();
	json["pressedTexture"] = pressedOpenGLTexture.GetTexturePath();
}

void C_Button::Load(Json& json)
{
	std::string path = json["idleTexture"].get<std::string>();
	SetIdleTexture(path.c_str());

	path = json["hoverTexture"].get<std::string>();
	SetHoverTexture(path.c_str());

	path = json["pressedTexture"].get<std::string>();
	SetPressedTexture(path.c_str());
}

void C_Button::Draw()
{
	R_Texture openGLTexture;

	switch (state) {
	case BUTTON_STATE::IDLE:
		openGLTexture = idleOpenGLTexture;
		break;
	case BUTTON_STATE::HOVER:
		openGLTexture = hoverOpenGLTexture;
		break;
	case BUTTON_STATE::PRESSED:
		openGLTexture = pressedOpenGLTexture;
		break;
	}

	owner->GetEngine()->GetUI()->PrepareUIRender();
	owner->GetComponent<C_Transform2D>()->drawablePlane->DrawPlane2D(openGLTexture.GetTextureId(), { 255, 255, 255 });
	owner->GetEngine()->GetUI()->EndUIRender();
}

void C_Button::SetIdleTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::IDLE);
	Importer::GetInstance()->textureImporter->Import(path,&idleOpenGLTexture);
}

void C_Button::SetHoverTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::HOVER);
	Importer::GetInstance()->textureImporter->Import(path, &hoverOpenGLTexture);
}

void C_Button::SetPressedTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::PRESSED);
	Importer::GetInstance()->textureImporter->Import(path, &pressedOpenGLTexture);
}

void C_Button::FreeTextures(BUTTON_STATE type)
{
	GLuint id = 0;
	switch (type) {
	case BUTTON_STATE::IDLE:
		//if (idleSDLTexture != nullptr)
		//	SDL_DestroyTexture(idleSDLTexture);
		id = idleOpenGLTexture.GetTextureId();
		if (id != 0)
			glDeleteTextures(1, &id);
		break;
	case BUTTON_STATE::HOVER:
		//if (hoverSDLTexture != nullptr)
		//	SDL_DestroyTexture(hoverSDLTexture);
		id = hoverOpenGLTexture.GetTextureId();
		if (id != 0)
			glDeleteTextures(1, &id);
		break;
	case BUTTON_STATE::PRESSED:
		//if (pressedSDLTexture != nullptr)
		//	SDL_DestroyTexture(pressedSDLTexture);
		id = pressedOpenGLTexture.GetTextureId();
		if (id != 0)
			glDeleteTextures(1, &id);
		break;
	}
}

bool C_Button::IsPressed()
{
	if (state == BUTTON_STATE::PRESSED && !isPressed)
	{
		isPressed = true;
		return true;
	}
	else if (state != BUTTON_STATE::PRESSED)
	{
		isPressed = false;
	}
	return false;
}

bool C_Button::IsHovered()
{
	if (state == BUTTON_STATE::HOVER && !isHovered)
	{
		isHovered = true;
		return true;
	}
	else if (state == BUTTON_STATE::IDLE)
	{
		isHovered = false;
	}
	return false;
}

bool C_Button::IsIdle()
{
	if (state == BUTTON_STATE::IDLE)
	{
		return true;
	}
	return false;
}

void C_Button::OnStoped()
{
	M_SceneManager* sceneManager = owner->GetEngine()->GetSceneManager();
	M_Camera3D* camera = owner->GetEngine()->GetCamera3D();
	sceneManager->OnStop();
	camera->OnStop();
}