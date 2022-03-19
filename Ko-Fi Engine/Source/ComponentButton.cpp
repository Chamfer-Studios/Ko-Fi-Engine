#include "ComponentButton.h"

#include "SDL.h"
#include "SDL_image.h"

#include "ImGuiAppLog.h"
#include "GameObject.h"
#include "Engine.h"
#include "UI.h"
#include "PanelChooser.h"
#include "ComponentTransform2D.h"
#include "ComponentCanvas.h"
#include "Input.h"
#include "Importer.h"

ComponentButton::ComponentButton(GameObject* parent) : Component(parent)
{
	type = ComponentType::BUTTON;
	glGenFramebuffers(1, &fboId);
}

ComponentButton::~ComponentButton()
{
	FreeTextures(BUTTON_STATE::IDLE);
	FreeTextures(BUTTON_STATE::HOVER);
	FreeTextures(BUTTON_STATE::PRESSED);
}

void ComponentButton::Save(Json& json) const
{
	json["type"] = "button";
	json["idleTexture"] = idleOpenGLTexture.path;
	json["hoverTexture"] = hoverOpenGLTexture.path;
	json["pressedTexture"] = pressedOpenGLTexture.path;
}

void ComponentButton::Load(Json& json)
{
	std::string path = json["idleTexture"].get<std::string>();
	SetIdleTexture(path.c_str());

	path = json["hoverTexture"].get<std::string>();
	SetHoverTexture(path.c_str());

	path = json["pressedTexture"].get<std::string>();
	SetPressedTexture(path.c_str());
}

bool ComponentButton::Update(float dt)
{
	float2 mouseScreenPosition = { (float)owner->GetEngine()->GetInput()->GetMouseX(), (float)owner->GetEngine()->GetInput()->GetMouseY() };
	ComponentCanvas* canvas = owner->GetComponent<ComponentTransform2D>()->GetCanvas();
	if (canvas != nullptr) {
		float2 mouseLogicalPosition = canvas->ScreenToLogical(mouseScreenPosition);
		bool mouseWithinBounds = owner->GetComponent<ComponentTransform2D>()->CheckPointWithinBounds(mouseLogicalPosition);

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

bool ComponentButton::PostUpdate(float dt)
{
	//SDL_Texture* SDLTexture = nullptr;
	Texture openGLTexture;

	switch (state) {
	case BUTTON_STATE::IDLE:
		//SDLTexture = idleSDLTexture;
		openGLTexture = idleOpenGLTexture;
		break;
	case BUTTON_STATE::HOVER:
		//SDLTexture = hoverSDLTexture;
		openGLTexture = hoverOpenGLTexture;
		break;
	case BUTTON_STATE::PRESSED:
		//SDLTexture = pressedSDLTexture;
		openGLTexture = pressedOpenGLTexture;
		break;
	}

	owner->GetEngine()->GetUI()->PrepareUIRender();
	owner->GetComponent<ComponentTransform2D>()->drawablePlane->DrawPlane2D(openGLTexture.GetTextureId(), {255, 255, 255});
	owner->GetEngine()->GetUI()->EndUIRender();

	return true;
}

bool ComponentButton::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Button")) {
		// IDLE
		ImGui::Text("IDLE: ");
		ImGui::SameLine();
		if (idleOpenGLTexture.GetTextureId() == 0)
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
			panelChooser->OpenPanel("IDLETextureButton", "png");
		}

		// HOVER
		ImGui::Text("HOVER: ");
		ImGui::SameLine();
		if (hoverOpenGLTexture.GetTextureId() == 0)
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
			panelChooser->OpenPanel("HOVERTextureButton", "png");
		}

		// PRESSED
		ImGui::Text("PRESSED: ");
		ImGui::SameLine();
		if (pressedOpenGLTexture.GetTextureId() == 0)
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
			panelChooser->OpenPanel("PRESSEDTextureButton", "png");
		}
	}

	return true;
}

/*SDL_Texture* ComponentButton::LoadTexture(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(path);

	if (surface == NULL)
	{
		appLog->AddLog("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

SDL_Texture* const ComponentButton::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(owner->GetEngine()->GetUI()->renderer, surface);

	if (texture == NULL)
	{
		appLog->AddLog("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		owner->GetEngine()->GetUI()->loadedTextures.push_back(texture);
	}

	return texture;
}*/

void ComponentButton::SetIdleTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::IDLE);
	Importer::GetInstance()->textureImporter->Import(path,&idleOpenGLTexture);
}

void ComponentButton::SetHoverTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::HOVER);
	Importer::GetInstance()->textureImporter->Import(path, &hoverOpenGLTexture);
}

void ComponentButton::SetPressedTexture(const char* path)
{
	FreeTextures(BUTTON_STATE::PRESSED);
	Importer::GetInstance()->textureImporter->Import(path, &pressedOpenGLTexture);
}

void ComponentButton::FreeTextures(BUTTON_STATE type)
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