#include "ComponentMaterial.h"
#include "Material.h"
#include "Log.h"

#include "Importer.h"
#include "I_Material.h"
#include "I_Texture.h"

#include "JsonHandler.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "FSDefs.h"

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

	material = nullptr;
	currentTextureId = 0;
}

ComponentMaterial::~ComponentMaterial()
{
	if(material != nullptr)
		RELEASE(material);
}

bool ComponentMaterial::CleanUp()
{
	if(material != nullptr)
		RELEASE(material);// peta por el karambit

	return true;
}

bool ComponentMaterial::Update(float dt)
{
	return true;
}

void ComponentMaterial::Save(Json& json) const
{
	json["type"] = "material";
	json["color"] = { material->diffuseColor.r,material->diffuseColor.g,material->diffuseColor.b,material->diffuseColor.a };
	json["shader_path"] = material->GetShaderPath();
	json["texture_path"] = texture.path;

	//json["material_path"] = material->GetMaterialPath();
	//json["material_name"] = material->materialName;

	//Json jsonTex;
	//json["textures"] = json::array();
	//for (auto tex : textures)
	//{
	//	jsonTex["path"] = tex.GetTexturePath();
	//	json["textures"].push_back(jsonTex);
	//}
	json["uniforms"].array();
	Json jsonUniform;
	for (Uniform* uniform : material->uniforms)
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
		default:
			continue;
		}
		json["uniforms"].push_back(jsonUniform);
	}
}

void ComponentMaterial::Load(Json& json)
{
	if (!json.empty())
	{
		if (material == nullptr)
			material = new Material();

		//material->materialName = json["material_name"];
		//material->SetMaterialPath(json.at("material_path").get<std::string>().c_str());

		std::string shaderPath = json.at("shader_path").get<std::string>();
		if (!shaderPath.empty())
			material->SetShaderPath(shaderPath.c_str());
		else
		{
			shaderPath = ASSETS_SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION;
			material->SetShaderPath(shaderPath.c_str());
		}

		if (!Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material))
		{
			CONSOLE_LOG("[ERROR] Something went wrong loading the shader.");
		}

		std::vector<float> values = json.at("color").get<std::vector<float>>();
		material->diffuseColor = Color(values[0], values[1], values[2], values[3]);
		values.clear();

		Importer::GetInstance()->textureImporter->Import(json.at("texture_path").get<std::string>().c_str(), &texture);
		//for (const auto& tex : json.at("textures").items())
		//{
		//	Texture t = Texture();
		//	Importer::GetInstance()->textureImporter->Import(tex.value().at("path").get<std::string>().c_str(), &t);
		//	textures.push_back(t);
		//}

		for (const auto& uni : json.at("uniforms").items())
		{
			std::string uniformName = uni.value().at("name").get<std::string>();
			uint uniformType = uni.value().at("type").get<uint>();
			switch (uniformType)
			{
			case GL_FLOAT:
			{
				UniformT<float>* uniform = (UniformT<float>*)material->FindUniform(uniformName);
				uniform->value = uni.value().at("value");
			}
			break;
			case GL_FLOAT_VEC2:
			{
				UniformT<float2>* uniform = (UniformT<float2>*)material->FindUniform(uniformName);
				uniform->value.x = uni.value().at("value").at("x");
				uniform->value.y = uni.value().at("value").at("y");
			}
			break;
			case GL_FLOAT_VEC3:
			{
				UniformT<float3>* uniform = (UniformT<float3>*)material->FindUniform(uniformName);
				uniform->value.x = uni.value().at("value").at("x");
				uniform->value.y = uni.value().at("value").at("y");
				uniform->value.z = uni.value().at("value").at("z");
			}
			break;
			case GL_FLOAT_VEC4:
			{
				UniformT<float4>* uniform = (UniformT<float4>*)material->FindUniform(uniformName);
				if (uniform)
				{
					uniform->value.x = uni.value().at("value").at("x");
					uniform->value.y = uni.value().at("value").at("y");
					uniform->value.z = uni.value().at("value").at("z");
					uniform->value.w = uni.value().at("value").at("w");
				}
			
			}
			break;
			case GL_INT:
			{
				UniformT<int>* uniform = (UniformT<int>*)material->FindUniform(uniformName);
				uniform->value = uni.value().at("value");
			}
			break;
			}
		}
	}
}

void ComponentMaterial::SetMaterial(Material* material)
{
	if (this->material != nullptr)
		RELEASE(this->material);

	this->material = material;
}

void ComponentMaterial::LoadMaterial(const char* path)
{
	/*std::string materialPath = path;

	JsonHandler jsonHandler;
	Json jsonMaterial;

	if (materialPath.empty())
		ret = jsonHandler.LoadJson(jsonMaterial, material->GetMaterialPath());
	else
		ret = jsonHandler.LoadJson(jsonMaterial, materialPath.c_str());

	if (!jsonMaterial.empty())
	{
		ret = true;
		material->materialName = jsonMaterial.at("name").get<std::string>();

		if (!materialPath.empty())
			material->SetMaterialPath(materialPath.c_str());

		std::vector<std::string> texturePaths = jsonMaterial.at("textures");
		for (int i = 0; i < texturePaths.size(); i++)
		{
			std::string texturePath = texturePaths.at(i);
			if (textures.empty())
			{
				Texture texture;
				Importer::GetInstance()->textureImporter->Import(texturePath.c_str(), &texture);
				textures.push_back(texture);
			}
			else if (texturePath != textures[i].GetTexturePath())
			{
				Texture texture;
				Importer::GetInstance()->textureImporter->Import(texturePath.c_str(), &texture);
				textures.push_back(texture);
			}
		}

		UniformT<float4>* albedoTint = (UniformT<float4>*)material->FindUniform("albedoTint");
		albedoTint->value =
		{
			jsonMaterial.at("uniforms").at("albedoTint").at("x"),
			jsonMaterial.at("uniforms").at("albedoTint").at("y"),
			jsonMaterial.at("uniforms").at("albedoTint").at("z"),
			jsonMaterial.at("uniforms").at("albedoTint").at("w"),
		};
	};*/
}

bool ComponentMaterial::LoadDefaultMaterial()
{
	bool ret = false;

	//std::string defaultMaterialPath = "Library/Materials/default.milk";

	//JsonHandler jsonHandler;
	//Json jsonMaterial;

	//ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath.c_str());
	
	//if (!jsonMaterial.empty())
	//{
	//	ret = true;
	//	Material* material = new Material();
	//	//material->SetShaderPath(defaultMaterialPath);
	//	Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(),material);
	//	this->SetMaterial(material);
	//	//material->materialName = jsonMaterial.at("name").get<std::string>();
	//	//material->SetMaterialPath(defaultMaterialPath.c_str());
	//	Texture texture;
	//	Importer::GetInstance()->textureImporter->Import(0, &texture);
	//	this->texture = texture;
	//	UniformT<float4>* albedoTint = (UniformT<float4>*)material->FindUniform("albedoTint");
	//	albedoTint->value =
	//	{
	//		jsonMaterial.at("uniforms").at("x"),
	//		jsonMaterial.at("uniforms").at("albedoTint").at("y"),
	//		jsonMaterial.at("uniforms").at("albedoTint").at("z"),
	//		jsonMaterial.at("uniforms").at("albedoTint").at("w"),
	//	};
	//}

	return ret;
}

bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material"))
	{
		//if (panelChooser->IsReadyToClose("AddTexture"))
		//{
		//	if (panelChooser->OnChooserClosed() != nullptr)
		//	{
		//		std::string path = panelChooser->OnChooserClosed();

		//		Texture tex = Texture();
		//		Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
		//		textures.push_back(texture);
		//	}
		//}

		if (panelChooser->IsReadyToClose("ChangeTexture"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();
				//for (Texture& tex : textures)
				//{
				//	if (tex.textureID == currentTextureId)
				//	{
				//		Texture tex;
				//		Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
				//		textures.push_back(texture);
				//	}
				//}
				if (!path.empty() && texture.textureID == currentTextureId)
				{
					texture.textureID = 0;
					texture.SetTexturePath(nullptr);

					Texture tex;
					Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
					texture = tex;
				}
			}
		}

		if (panelChooser->IsReadyToClose("ChangeShader"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();

				if (material->shaderProgramID != 0)
				{
					glDeleteProgram(material->shaderProgramID);
					material->uniforms.clear();
					material->uniforms.shrink_to_fit();
				}

				if (!path.empty())
					material->SetShaderPath(path.c_str());
				else
				{
					path = ASSETS_SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION;
					material->SetShaderPath(path.c_str());
				}
				Importer::GetInstance()->materialImporter->LoadAndCreateShader(path.c_str(), material);
			}
		}

		//ImGui::Text("Material Name:");
		//ImGui::SameLine();
		//ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), material->materialName.c_str());
		ImGui::Text("Material Texture:");
		//for (Texture& tex : textures)
		if (texture.textureID != -1)
		{
			ImGui::Image((ImTextureID)texture.textureID, ImVec2(85, 85));
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text(texture.GetTexturePath());
			ImGui::PushID(texture.textureID << 8);

			if (ImGui::Button("Change Texture"))
			{
				panelChooser->OpenPanel("ChangeTexture", "png", { "png","jpg","jpeg" });
				currentTextureId = texture.textureID;
			}

			ImGui::PopID();

			ImGui::PushID(texture.textureID << 16);

			if (ImGui::Button("Delete Texture"))
			{
				//material.textures.erase(std::remove(material.textures.begin(), material.textures.end(), tex));
				texture.textureID = -1;
				texture.SetTexturePath(nullptr);
			}
			ImGui::PopID();
			ImGui::EndGroup();
		}
		else
		{
			if (ImGui::Button("Add Texture"))
			{
				panelChooser->OpenPanel("ChangeTexture", "png", { "png","jpg","jpeg" });
				currentTextureId = texture.textureID;
			}
		}

		//if (ImGui::Button("Add Texture"))
		//	panelChooser->OpenPanel("AddTexture", "png");

		ImGui::Separator();

		if (ImGui::Button("Change Shader"))
			panelChooser->OpenPanel("ChangeShader", "glsl", { "glsl" });

		if (material != nullptr)
		{
			for (Uniform* uniform : material->uniforms)
			{
				switch (uniform->type)
				{
				case GL_FLOAT:
				{
					UniformT<float>* uf = (UniformT<float>*)uniform;
					ImGui::DragFloat(uniform->name.c_str(), &uf->value, 0.001f, 0.0f, 32.0f, "%.3f");
				}
				break;
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
					ImGui::DragFloat2(uniform->name.c_str(), uf2->value.ptr(), 0.001f, 0.0f, 32.0f, "%.3f");
				}
				break;
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
					ImGui::DragFloat3(uniform->name.c_str(), uf3->value.ptr(), 0.001f, -32.0f, 32.0f, "%.3f");
				}
				break;
				case GL_FLOAT_VEC4:
				{
					UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
					ImGui::DragFloat4(uniform->name.c_str(), uf4->value.ptr(), 0.001f, 0.0f, 1.0f, "%.3f");
				}
				break;
				case GL_INT:
				{
					UniformT<int>* ui = (UniformT<int>*)uniform;
					ImGui::DragInt(uniform->name.c_str(), &ui->value, 0.001f, 0.0f, 255.0f, "%d");
				}
				break;
				default:
					break;
				}
			}
		}
	}
	return true;
}