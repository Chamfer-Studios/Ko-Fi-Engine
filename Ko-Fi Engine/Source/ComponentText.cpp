#include "ComponentText.h"

#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "Engine.h"
#include "ImGuiAppLog.h"
#include "UI.h"

ComponentText::ComponentText(GameObject* parent) : Component(parent)
{
	type = ComponentType::TEXT;
	SetTextValue("Hello world!");
}

ComponentText::~ComponentText()
{
}

bool ComponentText::Update()
{
	return true;
}

bool ComponentText::PostUpdate(float dt)
{
	ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	SDL_Rect rect;

	rect.x = 30;
	rect.y = 30;

	if (SDLTexture != nullptr) {
		SDL_QueryTexture(SDLTexture, NULL, NULL, &rect.w, &rect.h);
	}
	else {
		rect.w = 0;
		rect.h = 0;
	}

	GLuint fboId = 0;

	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLTexture, 0);

	float2 normalizedPosition = cTransform->GetNormalizedPosition(true);
	float2 normalizedSize = cTransform->GetNormalizedSize(true);

	float2 lowerLeft = { normalizedPosition.x, normalizedPosition.y };
	float2 upperRight = { lowerLeft.x + normalizedSize.x, lowerLeft.y + normalizedSize.y };
	glBlitFramebuffer(0, 0, rect.w, rect.h, lowerLeft.x, lowerLeft.y, upperRight.x, upperRight.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return true;
}

bool ComponentText::InspectorDraw(PanelChooser* chooser)
{
	return true;
}

void ComponentText::SetTextValue(std::string newValue)
{
	textValue = newValue;
	SDL_Color orange = { 255, 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderUTF8_Blended(owner->GetEngine()->GetUI()->rubik, "Hello world!", orange);
	SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_BGRA8888, 0);
	openGLTexture = SurfaceToOpenGLTexture(converted);
	SDLTexture = SurfaceToSDLTexture(converted);
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

SDL_Texture* ComponentText::SurfaceToSDLTexture(SDL_Surface* surface)
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
}
