#include "C_Material.h"
#include "R_Material.h"
#include "Log.h"

// Modules
#include "Engine.h"
#include "M_Editor.h"
#include "M_ResourceManager.h"

// GameObject
#include "GameObject.h"

// Importers
#include "Importer.h"
#include "I_Material.h"
#include "I_Texture.h"
#include "R_Texture.h"

#include "JsonHandler.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "FSDefs.h"

#include "stb_image.h"
#include "imgui.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include <string>

#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

C_Material::C_Material(GameObject* parent) : Component(parent)
{
	type = ComponentType::MATERIAL;

	material = new R_Material();
	texture = new R_Texture();
	currentTextureId = 0;
}

C_Material::~C_Material()
{
	if(material != nullptr)
		RELEASE(material);
}

bool C_Material::CleanUp()
{
	if(material != nullptr)
		RELEASE(material);// peta por el karambit

	if (texture != nullptr)
		RELEASE(texture);// peta por el karambit

	return true;
}

bool C_Material::Update(float dt)
{
	return true;
}

void C_Material::Save(Json& json) const
{
	json["type"] = "material";

	if (material != nullptr)
	{
		json["color"] = {material->diffuseColor.r,material->diffuseColor.g,material->diffuseColor.b,material->diffuseColor.a};

		json["shader"]["asset_file"] = material->GetAssetFile();
		json["shader"]["asset_path"] = material->GetAssetPath();
		json["shader"]["library_file"] = material->GetLibraryFile();
		json["shader"]["library_path"] = material->GetLibraryPath();

		json["shader"]["uniforms"].array();
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
				break;
			}
			case GL_FLOAT_VEC2:
			{
				UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
				jsonUniform["name"] = uf2->name;
				jsonUniform["type"] = uf2->type;
				jsonUniform["value"]["x"] = uf2->value.x;
				jsonUniform["value"]["y"] = uf2->value.y;
				break;
			}
			case GL_FLOAT_VEC3:
			{
				UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
				jsonUniform["name"] = uf3->name;
				jsonUniform["type"] = uf3->type;
				jsonUniform["value"]["x"] = uf3->value.x;
				jsonUniform["value"]["y"] = uf3->value.y;
				jsonUniform["value"]["z"] = uf3->value.z;
				break;
			}
			case GL_FLOAT_VEC4:
			{
				UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
				jsonUniform["name"] = uf4->name;
				jsonUniform["type"] = uf4->type;
				jsonUniform["value"]["x"] = uf4->value.x;
				jsonUniform["value"]["y"] = uf4->value.y;
				jsonUniform["value"]["z"] = uf4->value.z;
				jsonUniform["value"]["w"] = uf4->value.w;
				break;
			}
			case GL_INT:
			{
				UniformT<int>* ui = (UniformT<int>*)uniform;
				jsonUniform["name"] = ui->name;
				jsonUniform["type"] = ui->type;
				jsonUniform["value"] = ui->value;
				break;
			}
			default:
				continue;
			}
			json["shader"]["uniforms"].push_back(jsonUniform);
		}
	}

	if (texture != nullptr)
	{
		json["texture"]["asset_path"] = texture->GetAssetPath();
		json["texture"]["asset_file"] = texture->GetAssetFile();
		json["texture"]["library_path"] = texture->GetLibraryPath();
		json["texture"]["library_file"] = texture->GetLibraryFile();
	}
}

void C_Material::Load(Json& json)
{
	if (!json.empty())
	{
		material = nullptr;
		texture = nullptr;

		Json jsonShader = json.at("shader");
		std::string materialAssetPath = jsonShader.at("asset_path").get<std::string>();
		material = (R_Material*)owner->GetEngine()->GetResourceManager()->GetResourceFromLibrary(materialAssetPath.c_str());

		if (material == nullptr)
			CONSOLE_LOG("[ERROR] Component Material: could not load resource from library.");
		else
		{
			material->SetAssetsPathAndFile(jsonShader.at("asset_path").get<std::string>().c_str(),
				jsonShader.at("asset_file").get<std::string>().c_str());
			material->SetAssetsPathAndFile(jsonShader.at("library_path").get<std::string>().c_str(),
				jsonShader.at("library_file").get<std::string>().c_str());

			std::vector<float> values = json.at("color").get<std::vector<float>>();
			material->diffuseColor = Color(values[0], values[1], values[2], values[3]);

			for (const auto& uni : jsonShader.at("uniforms").items())
			{
				std::string uniformName = uni.value().at("name").get<std::string>();
				uint uniformType = uni.value().at("type").get<uint>();
				switch (uniformType)
				{
				case GL_FLOAT:
				{
					UniformT<float>* uniform = (UniformT<float>*)material->FindUniform(uniformName);
					uniform->value = uni.value().at("value");
					break;
				}
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uniform = (UniformT<float2>*)material->FindUniform(uniformName);
					uniform->value.x = uni.value().at("value").at("x");
					uniform->value.y = uni.value().at("value").at("y");
					break;
				}
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uniform = (UniformT<float3>*)material->FindUniform(uniformName);
					uniform->value.x = uni.value().at("value").at("x");
					uniform->value.y = uni.value().at("value").at("y");
					uniform->value.z = uni.value().at("value").at("z");
					break;
				}
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

					break;
				}
				case GL_INT:
				{
					UniformT<int>* uniform = (UniformT<int>*)material->FindUniform(uniformName);
					uniform->value = uni.value().at("value");
					break;
				}
				}
			}
		}

		Json jsonTexture = json.at("texture");
		std::string textureAssetPath = jsonTexture.at("asset_path").get<std::string>();
		texture = (R_Texture*)owner->GetEngine()->GetResourceManager()->GetResourceFromLibrary(textureAssetPath.c_str());
		if (texture == nullptr)
			CONSOLE_LOG("[ERROR] Component Texture: could not load resource from library.");
		else
		{
			texture->SetAssetsPathAndFile(jsonTexture.at("asset_path").get<std::string>().c_str(),
				jsonTexture.at("asset_file").get<std::string>().c_str());
			texture->SetAssetsPathAndFile(jsonTexture.at("library_path").get<std::string>().c_str(),
				jsonTexture.at("library_file").get<std::string>().c_str());
		}
	}
}

void C_Material::SetMaterial(R_Material* material)
{
	if (this->material != nullptr)
		RELEASE(this->material);

	this->material = material;
}

void C_Material::LoadMaterial(const char* path)
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
				R_Texture texture;
				Importer::GetInstance()->textureImporter->Import(texturePath.c_str(), &texture);
				textures.push_back(texture);
			}
			else if (texturePath != textures[i].GetTexturePath())
			{
				R_Texture texture;
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

bool C_Material::LoadDefaultMaterial()
{
	bool ret = false;

	//std::string defaultMaterialPath = "Library/Materials/default.milk";

	//JsonHandler jsonHandler;
	//Json jsonMaterial;

	//ret = jsonHandler.LoadJson(jsonMaterial, defaultMaterialPath.c_str());
	
	//if (!jsonMaterial.empty())
	//{
	//	ret = true;
	//	R_Material* material = new R_Material();
	//	//material->SetShaderPath(defaultMaterialPath);
	//	Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(),material);
	//	this->SetMaterial(material);
	//	//material->materialName = jsonMaterial.at("name").get<std::string>();
	//	//material->SetMaterialPath(defaultMaterialPath.c_str());
	//	R_Texture texture;
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

bool C_Material::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);
		//if (panelChooser->IsReadyToClose("AddTexture"))
		//{
		//	if (panelChooser->OnChooserClosed() != nullptr)
		//	{
		//		std::string path = panelChooser->OnChooserClosed();

		//		R_Texture tex = R_Texture();
		//		Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
		//		textures.push_back(texture);
		//	}
		//}

		if (panelChooser->IsReadyToClose("ChangeTexture"))
		{
			if (panelChooser->OnChooserClosed() != nullptr)
			{
				std::string path = panelChooser->OnChooserClosed();
				//for (R_Texture& tex : textures)
				//{
				//	if (tex.textureID == currentTextureId)
				//	{
				//		R_Texture tex;
				//		Importer::GetInstance()->textureImporter->Import(path.c_str(), &tex);
				//		textures.push_back(texture);
				//	}
				//}
				if (!path.empty() && texture->GetTextureId() == currentTextureId)
				{
					texture->SetTextureId(0);
					texture->SetTexturePath(nullptr);

					R_Texture* tex = new R_Texture();
					Importer::GetInstance()->textureImporter->Import(path.c_str(), tex);
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
					path = ASSETS_SHADERS_DIR + std::string(DEFAULT_SHADER) + SHADER_EXTENSION;
					material->SetShaderPath(path.c_str());
				}
				Importer::GetInstance()->materialImporter->LoadAndCreateShader(path.c_str(), material);
			}
		}

		//ImGui::Text("Material Name:");
		//ImGui::SameLine();
		//ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), material->materialName.c_str());
		//ImGui::Text("Material Texture:");
		//for (R_Texture& tex : textures)
		if (texture->GetTextureId() != TEXTUREID_DEFAULT)
		{
			ImGui::Image((ImTextureID)texture->GetTextureId(), ImVec2(85, 85));
			ImGui::SameLine();
			ImGui::BeginGroup();

			if (texture->GetAssetPath() != nullptr) // We always should display assets path
			{
				ImGui::Text("Texture Path: ");
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
				if (ImGui::Selectable(texture->GetAssetPath())) {}
				ImGui::PopStyleColor();
			}

			ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

			if (ImGui::Button("Change Texture"))
			{
				panelChooser->OpenPanel("ChangeTexture", "png", { "png","jpg","jpeg" });
				currentTextureId = texture->GetTextureId();
			}

			ImGui::PopID();

			ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

			if (ImGui::Button("Delete Texture"))
			{
				//material.textures.erase(std::remove(material.textures.begin(), material.textures.end(), tex));
				texture->SetTextureId(TEXTUREID_DEFAULT);
				texture->SetTexturePath(nullptr);
			}
			ImGui::PopID();
			ImGui::EndGroup();
		}
		else
		{
			if (ImGui::Button("Add Texture"))
			{
				panelChooser->OpenPanel("ChangeTexture", "png", { "png","jpg","jpeg" });
				currentTextureId = texture->GetTextureId();
			}
		}

		//if (ImGui::Button("Add Texture"))
		//	panelChooser->OpenPanel("AddTexture", "png");

		ImGui::Separator();

		ImGui::Text("Shader Path: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		if (ImGui::Selectable(material->GetShaderPath())) {}
		ImGui::PopStyleColor();

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
	else
		DrawDeleteButton(owner, this);

	return true;
}