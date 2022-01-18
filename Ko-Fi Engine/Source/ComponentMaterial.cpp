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
	std::string defaultMaterialPath = "Assets/Materials/default.milk";
	JsonHandler jsonHandler;
	Json jsonMaterial;
	bool ret = true;

	ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath.c_str());

	if (!jsonMaterial.empty())
	{
		ret = true;
		material.materialName = jsonMaterial.at("name").get<std::string>();
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



void ComponentMaterial::LoadMaterial(const char* path)
{
	std::string materialPath = path;
	JsonHandler jsonHandler;
	Json jsonMaterial;
	bool ret = true;

	ret = jsonHandler.LoadJson(jsonMaterial, materialPath.c_str());

	if (!jsonMaterial.empty())
	{
		ret = true;
		material.materialName = jsonMaterial.at("name").get<std::string>();
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

void ComponentMaterial::Compile()
{
	LoadShader(this->shaderPath.c_str());
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
					//LoadTextureFromId(texture.textureID, path.c_str());
				LoadTexture(path.c_str());
			}
		}
		if (panelChooser->IsReadyToClose("MaterialComponentSelectShader")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				LoadShader(path.c_str());
			}
		}
		if (material.textureAlbedo.GetTexturePath() != nullptr && material.textureAlbedo.GetTexturePath() != "")
		{
			ImGui::TextColored(ImVec4(0, 1, 1, 1), "Material %s: ", material.materialName.c_str());
			
			ImGui::Columns(2, 0,false);	
			ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.30);
			if (ImGui::Button("Select Shader")) {
				panelChooser->OpenPanel("MaterialComponentSelectShader","glsl");
			}
			//Albedo Info
			ImGui::Text("Texture:");
			ImGui::SameLine();
			std::string texturePath = material.textureAlbedo.GetTexturePath();
			std::string textureName = texturePath.substr(texturePath.find_last_of("/\\")+1);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", textureName.c_str());
			
			////Metallic Info
			//ImGui::Text("Metallic:");
			//ImGui::SameLine();
			//ImGui::TextColored(ImVec4(1,1,0,1), "%d", material.metallicValue);
			////Roughness Info
			//ImGui::Text("Roughness:");
			//ImGui::SameLine();
			//ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", material.roughnessValue);
			////Ao Info
			//ImGui::Text("Ambient Occlusion:");
			//ImGui::SameLine();
			//ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", material.ambientOcclusionValue);
			//ImGui::NextColumn();

			//Material Texture Image
			ImGui::Image((ImTextureID)material.textureAlbedo.GetTextureId(), ImVec2(85, 85));
			ImGui::NextColumn();
			for (Uniform& uniform : uniforms) {
				if (uniform.type == GL_FLOAT_VEC4) {
					if (ImGui::SliderFloat4(uniform.name.c_str(), material.albedoTint.ptr(), 0, 1)) {
						Compile();
					}
				}
			}
			ImGui::Columns();
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

void ComponentMaterial::LoadUniforms()
{
	uniforms.clear();
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	glGetProgramiv(this->materialShader, GL_ACTIVE_UNIFORMS, &count);

	for (i = 0; i < count; i++) {

		glGetActiveUniform(this->materialShader, (GLuint)i, bufSize, &length, &size, &type, name);
		this->uniforms.push_back({ type, name});
	}
}
void ComponentMaterial::LoadShader(const char* shaderPath)
{
	if (this->materialShader != 0) {
		glDeleteProgram(this->materialShader);
	}
	shaderPath ? this->shaderPath = shaderPath : this->shaderPath = "Assets/Shaders/default_shader.glsl";
	shader::ShaderProgramSource shaderSource = shader::ParseShader(this->shaderPath);
	materialShader = shader::CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	LoadUniforms();
}



