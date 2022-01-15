#include "ComponentMaterial.h"
//#include "Mesh.h"
#include "stb_image.h"
#include "imgui.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "PanelChooser.h"
#include "Shader.h"
#include "JsonHandler.h"


ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = ComponentType::MATERIAL;
	LoadDefaultMaterial();
	LoadShader();
}

ComponentMaterial::~ComponentMaterial()
{
	glDeleteProgram(this->materialShader);

}

//void ComponentMaterial::LoadTextureFromId(uint& textureID, const char* path)
//{
//	this->path = path;
//	int width, height, nrChannels;
//	unsigned char* pixels = stbi_load(path, &width, &height, &nrChannels, STBI_rgb);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	// set the texture wrapping/filtering options (on the currently bound texture object)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	if (pixels)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		appLog->AddLog("%s", "Texture Image not loaded correctly");
//	}
//
//	stbi_image_free(pixels);
//}

void ComponentMaterial::LoadDefaultMaterial()
{
	const char* defaultMaterialPath = "Assets/Materials/default.milk";
	JsonHandler jsonHandler;
	Json jsonMaterial;
	bool ret = true;

	ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath);

	if (!jsonMaterial.empty())
	{
		ret = true;
		std::string path = jsonMaterial.at("albedo").at("texture").at("path").get<std::string>().c_str();
		LoadTexture(path.c_str());
		material.albedoTint = { jsonMaterial.at("albedo").at("color").at("r"),
			jsonMaterial.at("albedo").at("color").at("g"),
			jsonMaterial.at("albedo").at("color").at("b"),
			jsonMaterial.at("albedo").at("color").at("a"),
		};
		material.metallicValue = jsonMaterial.at("metallic").at("value");
		material.roughnessValue = jsonMaterial.at("roughness").at("value");
		material.ambientOcclusionValue = jsonMaterial.at("ao").at("value");
	};
}

void ComponentMaterial::LoadTexture(std::string path)
{
	this->material.textureAlbedo.SetUpTexture(path);
}



bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material")) {
		if (panelChooser->IsReadyToClose("MaterialComponent")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				path.erase(path.begin());
				
					//LoadTextureFromId(texture.textureID, path.c_str());
				LoadTexture(path.c_str());
			}
		}
		if (material.textureAlbedo.GetTexturePath() != nullptr && material.textureAlbedo.GetTexturePath() != "")
		{
			ImGui::Text("Texture Path: ");
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			if (ImGui::Selectable(material.textureAlbedo.GetTexturePath()))
			{
				panelChooser->OpenPanel("MaterialComponent","png");
			}
			ImGui::PopStyleColor();
			ImGui::Text("Texture width: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", material.textureAlbedo.GetTextureWidth());
			ImGui::Text("Texture height: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", material.textureAlbedo.GetTextureHeight());
			ImGui::Image((ImTextureID)material.textureAlbedo.GetTextureId(), ImVec2(85, 85));
		}
		if (ImGui::Button("Add Texture")) {
			panelChooser->OpenPanel("MaterialComponent","png");
		}
		if (ImGui::Button("Checkers Texture")) {
			
				LoadTexture();
			
		}
	}
	return true;
}

Texture ComponentMaterial::GetTexture()
{
	return material.textureAlbedo;
}

Material ComponentMaterial::GetMaterial()
{
	return this->material;
}

uint ComponentMaterial::GetShader()
{
	return materialShader;
}

void ComponentMaterial::LoadShader()
{
	shader::ShaderProgramSource shaderSource = shader::ParseShader("Assets/Shaders/default_shader.glsl");
	materialShader = shader::CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
}
