#include "ComponentMaterial.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Shader.h"

#include "JsonHandler.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"

#include "stb_image.h"
#include "imgui.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = ComponentType::MATERIAL;
	//LoadShader();
}

ComponentMaterial::~ComponentMaterial()
{
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
		material->materialName = jsonMaterial.at("name").get<std::string>();
		material->materialPath = defaultMaterialPath;
		LoadTexture(0);
		UniformT<float4>* albedoTint = (UniformT<float4>*)shader->FindUniform("albedoTint");
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
		ret = jsonHandler.LoadJson(jsonMaterial, material->materialPath.c_str());
	}
	else {
		ret = jsonHandler.LoadJson(jsonMaterial, materialPath.c_str());

	}


	if (!jsonMaterial.empty())
	{
		ret = true;
		material->materialName = jsonMaterial.at("name").get<std::string>();
		material->materialPath = materialPath.empty() ? material->materialPath : materialPath;
		std::vector<std::string> texturePaths = jsonMaterial.at("textures");
		for (int i = 0; i < texturePaths.size(); i++)
		{
			std::string texturePath = texturePaths.at(i);

			if (textures.empty())
			{
				LoadTexture(texturePath.c_str());
			}
			else
			{
				if (texturePath != textures[i].GetTexturePath())
					LoadTexture(texturePath.c_str());
			}
		}

		UniformT<float4>* albedoTint = (UniformT<float4>*)shader->FindUniform("albedoTint");
		albedoTint->value = { jsonMaterial.at("uniforms").at("albedoTint").at("x"),
			jsonMaterial.at("uniforms").at("albedoTint").at("y"),
			jsonMaterial.at("uniforms").at("albedoTint").at("z"),
			jsonMaterial.at("uniforms").at("albedoTint").at("w"),
		};

	};
}

void ComponentMaterial::Compile()
{
	//LoadShader(this->shaderPath.c_str());
}

void ComponentMaterial::LoadTexture(std::string path)
{
	Texture texture;
	texture.SetTexturePath(path.c_str());
	texture.SetUpTexture();
	textures.push_back(texture);
}

bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material"))
	{
		if (panelChooser->IsReadyToClose("AddTexture"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();

				//LoadTextureFromId(texture.textureID, path.c_str());

				LoadTexture(path.c_str());
			}
		}

		if (panelChooser->IsReadyToClose("ChangeTexture"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();

				//LoadTextureFromId(texture.textureID, path.c_str());

				for (Texture& tex : textures)
				{
					tex.SetTexturePath(path.c_str());
					if (tex.textureID == currentTextureId)
						tex.SetUpTexture();
				}
			}
		}

		if (panelChooser->IsReadyToClose("ChangeShader"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				//LoadShader(path.c_str());
			}
		}

		ImGui::Text("Material Name:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), material->materialName.c_str());
		ImGui::Text("Material Textures:");
		for (Texture& tex : textures)
		{
			ImGui::Image((ImTextureID)tex.textureID, ImVec2(85, 85));
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(tex.GetTexturePath());
			ImGui::PushID(tex.textureID << 8);

			if (ImGui::Button("Change Texture"))
			{
				panelChooser->OpenPanel("ChangeTexture", "png");
				currentTextureId = tex.textureID;
			}

			ImGui::PopID();

			ImGui::PushID(tex.textureID << 16);

			if (ImGui::Button("Delete Textures"))
			{
				//material.textures.erase(std::remove(material.textures.begin(), material.textures.end(), tex));
			}
			ImGui::PopID();
			ImGui::EndGroup();
		}

		if (ImGui::Button("Add Texture"))
			panelChooser->OpenPanel("AddTexture", "png");

		ImGui::Separator();

		if (ImGui::Button("Change Shader"))
			panelChooser->OpenPanel("ChangeShader", "glsl");

		for (Uniform* uniform : shader->uniforms)
		{
			switch (uniform->type)
			{
			case GL_FLOAT:
			{
				UniformT<float>* uf = (UniformT<float>*)uniform;
				ImGui::DragFloat(uniform->name.c_str(), &uf->value, -10, 10);
			}
			break;
			case GL_FLOAT_VEC2:
			{
				UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
				ImGui::DragFloat2(uniform->name.c_str(), uf2->value.ptr(), -10, 10);
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
			default:
				break;
			}
		}
	}
	return true;
}

Json ComponentMaterial::Save()
{
	Json jsonComponentMaterial;
	json jsonTex;
	jsonComponentMaterial["textures"] = json::array();
	for (Texture& tex : textures)
	{
		jsonTex["path"] = tex.GetTexturePath();
		jsonComponentMaterial["textures"].push_back(jsonTex);
	}

	json jsonUniform;
	for (Uniform* uniform : shader->uniforms)
	{
		switch (uniform->type)
		{
		case GL_FLOAT:
		{
			UniformT<float>* uf = (UniformT<float>*)uniform;
			jsonUniform["name"] = uf->name;
			jsonUniform["type"] = uf->type;
			jsonUniform["value"] = uf->value;
		}
		break;
		case GL_FLOAT_VEC2:
		{
			UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
			jsonUniform["name"] = uf2->name;
			jsonUniform["type"] = uf2->type;
			jsonUniform["value"]["x"] = uf2->value.x;
			jsonUniform["value"]["y"] = uf2->value.y;
		}
		break;
		case GL_FLOAT_VEC3:
		{
			UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
			jsonUniform["name"] = uf3->name;
			jsonUniform["type"] = uf3->type;
			jsonUniform["value"]["x"] = uf3->value.x;
			jsonUniform["value"]["y"] = uf3->value.y;
			jsonUniform["value"]["z"] = uf3->value.z;
		}
		break;
		case GL_FLOAT_VEC4:
		{
			UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
			jsonUniform["name"] = uf4->name;
			jsonUniform["type"] = uf4->type;
			jsonUniform["value"]["x"] = uf4->value.x;
			jsonUniform["value"]["y"] = uf4->value.y;
			jsonUniform["value"]["z"] = uf4->value.z;
			jsonUniform["value"]["w"] = uf4->value.w;
		}
		break;
		case GL_INT:
		{
			UniformT<int>* ui = (UniformT<int>*)uniform;
			jsonUniform["name"] = ui->name;
			jsonUniform["type"] = ui->type;
			jsonUniform["value"] = ui->value;
		}
		break;
		}
		jsonComponentMaterial["uniforms"].push_back(jsonUniform);

	}
	jsonComponentMaterial["materialName"] = GetMaterial()->materialName;
	jsonComponentMaterial["materialPath"] = GetMaterial()->materialPath;
	//jsonComponentMaterial["shaderPath"] = GetShaderPath();
	return jsonComponentMaterial;
}

Material* ComponentMaterial::GetMaterial()
{
	return material;
}


Shader* ComponentMaterial::GetShader()
{
	return shader;
}