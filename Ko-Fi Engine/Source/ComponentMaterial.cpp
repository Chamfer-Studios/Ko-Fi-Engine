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
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = ComponentType::MATERIAL;
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
	std::string defaultMaterialPath = "Library/Materials/default.milk";
	JsonHandler jsonHandler;
	Json jsonMaterial;
	bool ret = true;

	ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath.c_str());

	if (!jsonMaterial.empty())
	{
		ret = true;
		material.materialName = jsonMaterial.at("name").get<std::string>();
		material.materialPath = defaultMaterialPath;
		LoadTexture(0);
		UniformT<float4>* albedoTint = (UniformT<float4>*)material.FindUniform("albedoTint");
		albedoTint->value = { jsonMaterial.at("uniforms").at("x"),
			jsonMaterial.at("uniforms").at("albedoTint").at("y"),
			jsonMaterial.at("uniforms").at("albedoTint").at("z"),
			jsonMaterial.at("uniforms").at("albedoTint").at("w"),
		};
	};
}



void ComponentMaterial::LoadMaterial(const char* path)
{
	std::string materialPath = path;
	JsonHandler jsonHandler;
	Json jsonMaterial;
	bool ret = true;
	if (materialPath.empty()) {
		ret = jsonHandler.LoadJson(jsonMaterial, material.materialPath.c_str());
	}
	else {
		ret = jsonHandler.LoadJson(jsonMaterial, materialPath.c_str());

	}


	if (!jsonMaterial.empty())
	{
		ret = true;
		material.materialName = jsonMaterial.at("name").get<std::string>();
		material.materialPath = materialPath.empty() ? material.materialPath : materialPath;
		std::vector<std::string> texturePaths = jsonMaterial.at("textures");
		for (int i = 0; i < texturePaths.size(); i++) {
			std::string texturePath = texturePaths.at(i);
			if (material.textures.empty()) {
				LoadTexture(texturePath.c_str());
			}
			else {
				if (texturePath != material.textures[i].GetTexturePath())
					LoadTexture(texturePath.c_str());

			}
		}

		UniformT<float4>* albedoTint = (UniformT<float4>*)material.FindUniform("albedoTint");
		albedoTint->value = { jsonMaterial.at("uniforms").at("albedoTint").at("x"),
			jsonMaterial.at("uniforms").at("albedoTint").at("y"),
			jsonMaterial.at("uniforms").at("albedoTint").at("z"),
			jsonMaterial.at("uniforms").at("albedoTint").at("w"),
		};

	};
}

void ComponentMaterial::SaveMaterial()
{
}

void ComponentMaterial::Compile()
{
	LoadShader(this->shaderPath.c_str());
}

void ComponentMaterial::LoadTexture(std::string path)
{
	Texture texture;
	texture.SetUpTexture(path);
	material.textures.push_back(texture);
}



bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material")) {

		if (panelChooser->IsReadyToClose("AddTexture")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				LoadTexture(path.c_str());
			}
		}
		if (panelChooser->IsReadyToClose("ChangeTexture")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				for (Texture& tex : material.textures) {
					if (tex.GetTextureId() == currentTextureId)
						tex.SetUpTexture(path);
				}
			}
		}
		if (panelChooser->IsReadyToClose("ChangeShader")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				LoadShader(path.c_str());
			}
		}

		ImGui::Text("Material Name:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), material.materialName.c_str());
		ImGui::Text("Material Textures:");
		for (Texture& tex : material.textures) {
			ImGui::Image((ImTextureID)tex.GetTextureId(), ImVec2(85, 85));
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(tex.GetTexturePath());
			ImGui::PushID(tex.GetTextureId() << 8);
			if (ImGui::Button("Change Texture")) {
				panelChooser->OpenPanel("ChangeTexture", "png");
				currentTextureId = tex.GetTextureId();
			}
			ImGui::PopID();

			ImGui::PushID(tex.GetTextureId() << 16);
			if (ImGui::Button("Delete Textures")) {
				material.textures.erase(std::remove(material.textures.begin(), material.textures.end(), tex));
			}
			ImGui::PopID();
			ImGui::EndGroup();
		}
		if (ImGui::Button("Add Texture")) {
			panelChooser->OpenPanel("AddTexture", "png");

		}
		ImGui::Separator();
		if (ImGui::Button("Change Shader")) {
			panelChooser->OpenPanel("ChangeShader","glsl");
		}
		for (Uniform* uniform : material.uniforms) {
			switch (uniform->type) {
			case GL_FLOAT:
			{
				if (uniform->name != "time") {
					UniformT<float>* uf = (UniformT<float>*)uniform;
					ImGui::DragFloat(uniform->name.c_str(), &uf->value, -10, 10);
				}
				
			}
			break;
			case GL_FLOAT_VEC2:
			{
				if (uniform->name != "resolution") {
					UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
					ImGui::DragFloat2(uniform->name.c_str(), uf2->value.ptr(), -10, 10);
				}
				
			}
			break;
			case GL_FLOAT_VEC3:
			{
					UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
					ImGui::DragFloat3(uniform->name.c_str(), uf3->value.ptr(), -10, 10);
				

			}
			break;
			case GL_FLOAT_VEC4:
			{

				UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
				ImGui::DragFloat4(uniform->name.c_str(), uf4->value.ptr(), -10, 10);
			}
			break;
			case GL_INT:

			{
				UniformT<int>* ui = (UniformT<int>*)uniform;
				ImGui::DragInt(uniform->name.c_str(), &ui->value, -10, 10);
			}
			break;
			}
		}

	}
	return true;
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
	this->material.uniforms.clear();
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
		switch (type) {
		case GL_FLOAT:
		{
			UniformT<float>* uf = new UniformT<float>(name, type, 0.0f);
			this->material.AddUniform(uf);
		}
		break;
		case GL_FLOAT_VEC2:
		{
			UniformT<float2>* uf2 = new UniformT<float2>(name, type, float2(1.0f,1.0f));
			this->material.AddUniform(uf2);
		}
		break;
		case GL_FLOAT_VEC3:
		{
			UniformT<float3>* uf3 = new UniformT<float3>(name, type, float3(1.0f,1.0f,1.0f));
			this->material.AddUniform(uf3);
		}
		break;
		case GL_FLOAT_VEC4:
		{

			UniformT<float4>* uf4 = new UniformT<float4>(name, type, float4(1.0f, 1.0f, 1.0f, 1.0f));
			this->material.AddUniform(uf4);
		}
		break;
		case GL_INT:

		{
			UniformT<int>* ui = new UniformT<int>(name, type, 0);
			this->material.AddUniform(ui);
		}
		break;
		}
	}
}
void ComponentMaterial::LoadShader(const char* shaderPath)
{
	if (this->materialShader != 0) {
		glDeleteProgram(this->materialShader);
	}
	shaderPath ? this->shaderPath = shaderPath : this->shaderPath = "Library/Shaders/default_shader.glsl";
	shader::ShaderProgramSource shaderSource = shader::ParseShader(this->shaderPath);
	materialShader = shader::CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	LoadUniforms();
}



