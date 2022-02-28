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
}

ComponentMaterial::~ComponentMaterial()
{
}

bool ComponentMaterial::LoadDefaultMaterial()
{
	bool ret = false;

	std::string defaultMaterialPath = "Library/Materials/default.milk";

	JsonHandler jsonHandler;
	Json jsonMaterial;

	ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath.c_str());

	if (!jsonMaterial.empty())
	{
		ret = true;
		material->materialName = jsonMaterial.at("name").get<std::string>();
		material->materialPath = defaultMaterialPath;
		LoadTexture(0);
		UniformT<float4>* albedoTint = (UniformT<float4>*)shader->FindUniform("albedoTint");
		albedoTint->value = {
			jsonMaterial.at("uniforms").at("x"),
			jsonMaterial.at("uniforms").at("albedoTint").at("y"),
			jsonMaterial.at("uniforms").at("albedoTint").at("z"),
			jsonMaterial.at("uniforms").at("albedoTint").at("w"),
		};
	};

	return ret;
}

void ComponentMaterial::LoadMaterial(const char* path)
{
	bool ret = false;

	std::string materialPath = path;

	JsonHandler jsonHandler;
	Json jsonMaterial;

	if (materialPath.empty())
		ret = jsonHandler.LoadJson(jsonMaterial, material->materialPath.c_str());
	else
		ret = jsonHandler.LoadJson(jsonMaterial, materialPath.c_str());

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
		albedoTint->value = {
			jsonMaterial.at("uniforms").at("albedoTint").at("x"),
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

void ComponentMaterial::Save(Json& json) const
{
	json["type"] = "material";
	json["color"] = { material->diffuseColor.r,material->diffuseColor.g,material->diffuseColor.b,material->diffuseColor.a };
	json["material_path"] = material->materialPath;
	json["material_name"] = material->materialName;
	json["shader_path"] = shader->GetShaderPath();

	json["textures"] = json::array();
	for (std::vector<Texture>::const_iterator tex = textures.begin(); tex != textures.end(); ++tex)
	{
		Texture t = (*tex);
		json["textures"]["path"] = t.GetTexturePath();
	}

	for (Uniform* uniform : shader->uniforms)
	{
		switch (uniform->type)
		{
		case GL_FLOAT:
		{
			UniformT<float>* uf = (UniformT<float>*)uniform;
			json["uniforms"]["name"] = uf->name;
			json["uniforms"]["type"] = uf->type;
			json["uniforms"]["value"] = uf->value;
		}
		break;
		case GL_FLOAT_VEC2:
		{
			UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
			json["uniforms"]["name"] = uf2->name;
			json["uniforms"]["type"] = uf2->type;
			json["uniforms"]["value"]["x"] = uf2->value.x;
			json["uniforms"]["value"]["y"] = uf2->value.y;
		}
		break;
		case GL_FLOAT_VEC3:
		{
			UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
			json["uniforms"]["name"] = uf3->name;
			json["uniforms"]["type"] = uf3->type;
			json["uniforms"]["value"]["x"] = uf3->value.x;
			json["uniforms"]["value"]["y"] = uf3->value.y;
			json["uniforms"]["value"]["z"] = uf3->value.z;
		}
		break;
		case GL_FLOAT_VEC4:
		{
			UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
			json["uniforms"]["name"] = uf4->name;
			json["uniforms"]["type"] = uf4->type;
			json["uniforms"]["value"]["x"] = uf4->value.x;
			json["uniforms"]["value"]["y"] = uf4->value.y;
			json["uniforms"]["value"]["z"] = uf4->value.z;
			json["uniforms"]["value"]["w"] = uf4->value.w;
		}
		break;
		case GL_INT:
		{
			UniformT<int>* ui = (UniformT<int>*)uniform;
			json["uniforms"]["name"] = ui->name;
			json["uniforms"]["type"] = ui->type;
			json["uniforms"]["value"] = ui->value;
		}
		break;
		}
	}
}

void ComponentMaterial::Load(Json& json)
{
	// TODO: load what we are saving
}

Material* ComponentMaterial::GetMaterial()
{
	return material;
}


Shader* ComponentMaterial::GetShader()
{
	return shader;
}