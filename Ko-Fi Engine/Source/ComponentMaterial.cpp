#include "ComponentMaterial.h"
#include "stb_image.h"
#include "imgui.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ComponentMaterial::ComponentMaterial() : Component(COMPONENT_TYPE::COMPONENT_MATERIAL)
{
	path = "";
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::LoadTexture(uint& textureID, const char* path)
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

bool ComponentMaterial::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::Text("Texture Path: ");
		ImGui::SameLine();
		if (ImGui::Selectable(path))
		{
		}
		if (ImGui::Button("Add Texture")) {
			for (uint& textureID : textureIds)
			{
				LoadTexture(textureID, "Assets/Images/baker_house.png");
			}
		}
	}
	return true;
}

void ComponentMaterial::AddTextureId(uint& textureID)
{
	textureIds.push_back(textureID);
}
