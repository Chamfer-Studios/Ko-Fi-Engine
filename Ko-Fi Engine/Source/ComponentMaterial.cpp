#include "ComponentMaterial.h"
//#include "Mesh.h"
#include "stb_image.h"
#include "imgui.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "PanelChooser.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	path = "";

}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::LoadTextureFromId(uint& textureID, const char* path)
{
	this->path = path;
	int width, height, nrChannels;
	unsigned char* pixels = stbi_load(path, &width, &height, &nrChannels, STBI_rgb);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		appLog->AddLog("%s", "Texture Image not loaded correctly");
	}

	stbi_image_free(pixels);
}

void ComponentMaterial::LoadTexture(const char* path)
{

		this->path = path;

		unsigned char* pixels = stbi_load(path, &texture.width, &texture.height, &texture.nrChannels, STBI_rgb);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture.textureID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (pixels)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			appLog->AddLog("%s", "Texture Image not loaded correctly");
		}

		stbi_image_free(pixels);
	
}

void ComponentMaterial::LoadDefaultTexture(uint& textureID)
{
	this->path = "";
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(textureID, 0);
}

bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material")) {
		if (panelChooser->IsReadyToClose("MaterialComponent")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				path = panelChooser->OnChooserClosed();
				path.erase(path.begin());
				
					LoadTextureFromId(texture.textureID, path.c_str());
				
			}
		}
		if (path.c_str() != nullptr && path.c_str() != "")
		{
			ImGui::Text("Texture Path: ");
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			if (ImGui::Selectable(path.c_str()))
			{
				panelChooser->OpenPanel("MaterialComponent","png");
			}
			ImGui::PopStyleColor();
			ImGui::Text("Texture width: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", texture.width);
			ImGui::Text("Texture height: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", texture.height);
			ImGui::Image((ImTextureID)texture.textureID, ImVec2(85, 85));
		}
		if (ImGui::Button("Add Texture")) {
			panelChooser->OpenPanel("MaterialComponent","png");
		}
		if (ImGui::Button("Checkers Texture")) {
			
				LoadDefaultTexture(texture.textureID);
			
		}
	}
	return true;
}

void ComponentMaterial::AddTextures(Texture texture)
{
	this->texture = texture;
}