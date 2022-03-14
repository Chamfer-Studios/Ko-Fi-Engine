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
}

ComponentText::~ComponentText()
{
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
	ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = texW;
	rect.h = texH;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLTexture, 0);

	float2 normalizedPosition = cTransform->GetNormalizedPosition(true);
	float2 normalizedSize = cTransform->GetNormalizedSize(true);

	float2 lowerLeft = { normalizedPosition.x, normalizedPosition.y };
	float2 upperRight = { lowerLeft.x + normalizedSize.x, lowerLeft.y + normalizedSize.y };
	glBlitFramebuffer(0, 0, rect.w, rect.h, lowerLeft.x, lowerLeft.y, upperRight.x, upperRight.y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

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

	textValue = newValue;
	SDL_Color color = { 255, 255, 255, 255 };
	uint colorAsDecimal = (color.r << 24) + (color.g << 16) + (color.b << 8) + (color.a);
	SDL_Surface* srcSurface = TTF_RenderUTF8_Blended(owner->GetEngine()->GetUI()->rubik, textValue.c_str(), color);

	if (srcSurface == nullptr)
		appLog->AddLog("%s\n", SDL_GetError());

	SDL_Surface* dstSurface = SDL_CreateRGBSurface(0, (srcSurface != nullptr ? srcSurface->w : 100), (srcSurface != nullptr ? srcSurface->h : 100), 32, 0xff, 0xff00, 0xff0000, 0xff000000);

	texW = dstSurface->w;
	texH = dstSurface->h;

	openGLTexture = SurfaceToOpenGLTexture(srcSurface);

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);
	glDisable(GL_BLEND);

	return id;
}

void ComponentText::FreeTextures()
{
	if (openGLTexture != 0)
		glDeleteTextures(1, &openGLTexture);
}