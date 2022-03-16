#include "ComponentText.h"

#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "Engine.h"
#include "ImGuiAppLog.h"
#include "imgui_stdlib.h"
#include "Editor.h"
#include "UI.h"

ComponentText::ComponentText(GameObject* parent) : Component(parent)
{
	type = ComponentType::TEXT;
	SetTextValue("Hello world!");
	glGenFramebuffers(1, &fboId);
	drawablePlane = new MyPlane(owner);
}

ComponentText::~ComponentText()
{
	delete drawablePlane;
	FreeTextures();
}

void ComponentText::Save(Json& json) const
{
	json["type"] = "text";
	json["value"] = textValue;
}

void ComponentText::Load(Json& json)
{
	std::string value = json["value"].get<std::string>();
	SetTextValue(value);
}

bool ComponentText::Update(float dt)
{
	return true;
}

bool ComponentText::PostUpdate(float dt)
{
	owner->GetEngine()->GetUI()->PrepareUIRender();
	drawablePlane->DrawPlane2D(openGLTexture, { 255, 255, 255 });
	owner->GetEngine()->GetUI()->EndUIRender();

	return true;
}

bool ComponentText::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Text")) {
		if (ImGui::InputText("Value", &(textValue))) {
			SetTextValue(textValue);
		}
	}

	return true;
}

void ComponentText::SetTextValue(std::string newValue)
{
	FreeTextures();

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	textValue = newValue;
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface* srcSurface = TTF_RenderUTF8_Blended(owner->GetEngine()->GetUI()->rubik, textValue.c_str(), color);

	if (srcSurface == nullptr)
		appLog->AddLog("%s\n", SDL_GetError());

	SDL_Surface* dstSurface = SDL_CreateRGBSurface(0, (srcSurface != nullptr ? srcSurface->w : 100), (srcSurface != nullptr ? srcSurface->h : 100), 32, 0xff, 0xff00, 0xff0000, 0xff000000);

	if (srcSurface != nullptr) {
		SDL_BlitSurface(srcSurface, NULL, dstSurface, NULL);
	}
	else {
		SDL_Rect rect = { 0, 0, dstSurface->w, dstSurface->h };
		Uint32 black = (255 << 24) + (0 << 16) + (0 << 8) + (0);
		SDL_FillRect(dstSurface, &rect, black);
	}

	openGLTexture = SurfaceToOpenGLTexture(dstSurface);

	SDL_FreeSurface(srcSurface);
	SDL_FreeSurface(dstSurface);
}

GLuint ComponentText::SurfaceToOpenGLTexture(SDL_Surface* surface)
{
	GLenum textureFormat;
	Uint8 nOfColors = surface->format->BytesPerPixel;
	if (nOfColors == 4)     // contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGBA;
		else
			textureFormat = GL_BGRA;
	}
	else if (nOfColors == 3)     // no alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
			textureFormat = GL_RGB;
		else
			textureFormat = GL_BGR;
	}
	else {
		appLog->AddLog("warning: the image is not truecolor..  this will probably break\n");
	}

	GLuint id;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	return id;
}

void ComponentText::FreeTextures()
{
	if (openGLTexture != 0)
		glDeleteTextures(1, &openGLTexture);
}