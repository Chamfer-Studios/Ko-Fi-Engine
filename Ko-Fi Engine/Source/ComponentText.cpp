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
	if (srcSurface != nullptr) {
		SDL_BlitSurface(srcSurface, NULL, dstSurface, NULL);
	}
	else {
		SDL_Rect rect = { 0, 0, dstSurface->w, dstSurface->h };
		Uint32 black = (255 << 24) + (0 << 16) + (0 << 8) + (0);
		SDL_FillRect(dstSurface, &rect, black);
	}

	FILE* dstSurfaceFile = fopen("Assets/outputdst.txt", "wt");
	int i, j, k = 0;
	for (i = 0; i < dstSurface->w; i++)
	{
		for (j = 0; j < dstSurface->h; j++)
		{
			fprintf(dstSurfaceFile, "%u %u %u %u ", (unsigned int)((unsigned char*)dstSurface->pixels)[k], (unsigned int)((unsigned char*)dstSurface->pixels)[k + 1], (unsigned int)((unsigned char*)dstSurface->pixels)[k + 2], (unsigned int)((unsigned char*)dstSurface->pixels)[k + 3]);
			k = k + 4;
		}
		fprintf(dstSurfaceFile, "\n");
	}

	texW = dstSurface->w;
	texH = dstSurface->h;

	openGLTexture = SurfaceToOpenGLTexture(dstSurface);
	//SDLTexture = SurfaceToSDLTexture(dstSurface);

	SaveToFile(dstSurface->w, dstSurface->h);

	SDL_FreeSurface(srcSurface);
	SDL_FreeSurface(dstSurface);
}

void ComponentText::SaveToFile(int width, int height)
{
	FILE* output_image;
	int output_width, output_height;

	output_width = width;
	output_height = height;

	/// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
	int             i, j, k;
	unsigned char* pixels = (unsigned char*)malloc(output_width * output_height * 4);

	/// READ THE CONTENT FROM THE FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, output_width, output_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	output_image = fopen("Assets/output.txt", "wt");

	k = 0;
	for (i = 0; i < output_width; i++)
	{
		for (j = 0; j < output_height; j++)
		{
			fprintf(output_image, "%u %u %u %u ", (unsigned int)pixels[k], (unsigned int)pixels[k + 1], (unsigned int)pixels[k + 2], (unsigned int)pixels[k + 3]);
			k = k + 4;
		}
		fprintf(output_image, "\n");
	}
	free(pixels);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

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

void ComponentText::FreeTextures()
{
	//if (SDLTexture != nullptr)
	//	SDL_DestroyTexture(SDLTexture);
	if (openGLTexture != 0)
		glDeleteTextures(1, &openGLTexture);
}