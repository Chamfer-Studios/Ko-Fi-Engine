#include "M_MouseGraphics.h"
#include "Engine.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Editor.h"
#include "R_Texture.h"
#include "Importer.h"
#include "MathGeoLib/Math/float2.h"
#include "glew.h"
M_MouseGraphics::M_MouseGraphics(KoFiEngine* engine)
{
	this->engine = engine;
	currentMouseTexture = new R_Texture();
}

M_MouseGraphics::~M_MouseGraphics()
{
}

bool M_MouseGraphics::Start()
{
	return true;
}

bool M_MouseGraphics::CleanUp()
{
	return true;
}

bool M_MouseGraphics::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_MouseGraphics::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_MouseGraphics::InspectorDraw()
{
	return true;
}

void M_MouseGraphics::OnNotify(const Event& event)
{
}

void M_MouseGraphics::SetMouseTexture(std::string texturePath)
{
	currentMouseTexture->DeleteTexture();
	Importer::GetInstance()->textureImporter->Import("",currentMouseTexture);

}


void M_MouseGraphics::RenderMouseTexture()
{
	if (currentMouseTexture->GetTextureId() != TEXTUREID_DEFAULT)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, engine->GetWindow()->GetWidth(), engine->GetWindow()->GetHeight(), 0.0f, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		POINT mousePosition;
		ShowCursor(false);
		GetCursorPos(&mousePosition);
		glLoadIdentity();
		glTranslatef(mousePosition.x, mousePosition.y, 0);
		glBindTexture(GL_TEXTURE_2D, currentMouseTexture->textureID);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(mousePosition.x, mousePosition.y, 0.0f);
		glTexCoord2f(0.0, 2.0); glVertex3f(mousePosition.x, mousePosition.y + currentMouseTexture->height, 0.0f);
		glTexCoord2f(2.0, 2.0); glVertex3f(mousePosition.x + currentMouseTexture->width, mousePosition.y + currentMouseTexture->height, 0.0f);
		glTexCoord2f(2.0, 0.0); glVertex3f(mousePosition.x + currentMouseTexture->width, mousePosition.y, 0.0f);
		glEnd();
	}
	
}
