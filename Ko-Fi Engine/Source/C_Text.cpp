#include "C_Text.h"

#include "GameObject.h"
#include "C_Transform2D.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "Engine.h"
#include "ImGuiAppLog.h"
#include "imgui_stdlib.h"
#include "M_Editor.h"
#include "M_UI.h"
#include "PanelChooser.h"

#include <vector>

C_Text::C_Text(GameObject* parent) : C_RenderedUI(parent)
{
	type = ComponentType::TEXT;
	size = 1;
	SetTextValue("Hello world!");
}

C_Text::~C_Text()
{
	FreeTextures();
}

bool C_Text::CleanUp()
{
	FreeTextures();
	return true;
}

void C_Text::Save(Json& json) const
{
	std::vector<int> color;

	color.push_back(col.r);
	color.push_back(col.g);
	color.push_back(col.b);
	color.push_back(col.a);

	json["type"] = (int)type;
	json["value"] = textValue;
	json["color"] = color;
}

void C_Text::Load(Json& json)
{
	std::vector<int> color;
	if (json.find("color") != json.end()) {
		color = json["color"].get<std::vector<int>>();

		col.r = color[0];
		col.g = color[1];
		col.b = color[2];
		col.a = color[3];
	}
	else {
		col.r = 255;
		col.g = 255;
		col.b = 255;
		col.a = 255;
	}

	std::string value = json["value"].get<std::string>();
	SetTextValue(value);
}

bool C_Text::Update(float dt)
{
	return true;
}

bool C_Text::PostUpdate(float dt)
{
	return true;
}

bool C_Text::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Text", ImGuiTreeNodeFlags_AllowItemOverlap)) 
	{
		if (DrawDeleteButton(owner, this))
			return true;

		if (ImGui::InputText("Value", &(textValue))) 
		{
			SetTextValue(textValue);
		}

		if (panelChooser->IsReadyToClose("AddFont")) {
			if (!panelChooser->OnChooserClosed().empty()) {
				std::string path = panelChooser->OnChooserClosed();
				SetFont(path.c_str());
				SetTextValue(textValue);
			}
		}

		if (ImGui::Button("Set Font")) {
			panelChooser->OpenPanel("AddFont", "ttf", { "ttf" });
		}

		SDL_Color tmpcol = GetColor();
		float c[4] = { tmpcol.r / 255.0, tmpcol.g / 255.0, tmpcol.b / 255.0, tmpcol.a / 255.0 };

		if (ImGui::ColorEdit4("Text Color", c, ImGuiColorEditFlags_DefaultOptions_) != false)
		{
			tmpcol.r = c[0] * 255;
			tmpcol.g = c[1] * 255;
			tmpcol.b = c[2] * 255;
			tmpcol.a = c[3] * 255;
			SetColor(tmpcol);
			SetTextValue(textValue);
		}
	}
	else
		DrawDeleteButton(owner, this);

	
	

	return true;
}

void C_Text::SetTextValue(std::string newValue)
{
	FreeTextures();

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if (selectedFont == nullptr)
	{
		selectedFont = owner->GetEngine()->GetUI()->rubik;
	}

	textValue = newValue;

	SDL_Surface* srcSurface = TTF_RenderUTF8_Blended(selectedFont, textValue.c_str(), col);

	if (srcSurface == nullptr)
		appLog->AddLog("%s\n", SDL_GetError());

	SDL_Surface* dstSurface = SDL_CreateRGBSurface(0, (srcSurface != nullptr ? srcSurface->w : 100), (srcSurface != nullptr ? srcSurface->h : 100), 32, 0xff, 0xff00, 0xff0000, 0xff000000);

	if (srcSurface != nullptr) {
		SDL_BlitSurface(srcSurface, NULL, dstSurface, NULL);
	}
	else {
		SDL_Rect rect = { 0, 0, dstSurface->w * size, dstSurface->h * size };
		Uint32 black = (255 << 24) + (0 << 16) + (0 << 8) + (0);
		SDL_FillRect(dstSurface, &rect, black);
	}

	openGLTexture = SurfaceToOpenGLTexture(dstSurface);

	int w, h;
	TTF_SizeUTF8(selectedFont, newValue.c_str(), &w, &h);
	owner->GetComponent<C_Transform2D>()->SetSize({ (float)w, (float)h });

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	SDL_FreeSurface(srcSurface);
	SDL_FreeSurface(dstSurface);
}

void C_Text::SetFont(std::string path)
{
	selectedFont = TTF_OpenFont(path.c_str(), 60);
}

void C_Text::SetSize(int newSize)
{
	this->size = newSize;
}

void C_Text::Draw()
{
	owner->GetEngine()->GetUI()->PrepareUIRender();
	owner->GetComponent<C_Transform2D>()->drawablePlane->DrawPlane2D(openGLTexture, { 255, 255, 255 });
	owner->GetEngine()->GetUI()->EndUIRender();
}

GLuint C_Text::SurfaceToOpenGLTexture(SDL_Surface* surface)
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

void C_Text::FreeTextures()
{
	if (openGLTexture != 0)
		glDeleteTextures(1, &openGLTexture);
}


SDL_Color C_Text::GetColor()
{
	return col;
}

void C_Text::SetColor(SDL_Color color)
{
	col.r = color.r;
	col.g = color.g;
	col.b = color.b;
	col.a = color.a;
}