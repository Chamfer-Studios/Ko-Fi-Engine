#include "I_Material.h"
#include "Material.h"
#include "Log.h"

I_Material::I_Material()
{
}

I_Material::~I_Material()
{
}

bool I_Material::Import(const aiMaterial* aiMaterial, Material* material)
{
	//if (material == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Import Material! Error: R_Material* was nullptr.");
	//	return;
	//}
	//if (aiMaterial == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Import Material { %s }! Error: aiMaterial* was nullptr.", material->GetAssetsFile());
	//	return;
	//}


	//std::string dirPath = ASSETS_TEXTURES_PATH + App->fileSystem->GetLastDirectory(material->GetAssetsPath());			// Dirty setting of the assets path.

	//std::string file = "";
	//std::string fullPath = "";

	//aiColor4D color;
	//if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)															// Could also get specular and ambient occlusion colours.
	//{
	//	material->diffuseColor = Color(color.r, color.g, color.b, color.a);
	//}

	//aiString texPath;
	//if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)												// At the moment only DIFFUSE textures will be imported.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::DIFFUSE, 0, fullPath));

	//}
	//if (aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS)											// Checking if there is a SPECULAR texture.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::SPECULAR, 0, fullPath));
	//}
	//if (aiMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texPath) == AI_SUCCESS)												// Checking if there is a AMBIENT texture.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::AMBIENT, 0, fullPath));

	//}
	//if (aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texPath) == AI_SUCCESS)											// Checking if there is a EMISSIVE texture.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::EMISSIVE, 0, fullPath));
	//}
	//if (aiMaterial->GetTexture(aiTextureType_HEIGHT, 0, &texPath) == AI_SUCCESS)												// Checking if there is a HEIGHT texture.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::HEIGHT, 0, fullPath));

	//}
	//if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS)												// Checking if there is a NORMALS texture.
	//{

	//	file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
	//	fullPath = dirPath + file;
	//	material->materials.push_back(MaterialData(TextureType::NORMALS, 0, fullPath));

	//}
	return true;
}

bool I_Material::Save(const Material* material, char** buffer)
{
	//uint written = 0;

	//if (material == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Save Material to Library! Error: R_Material* was nullptr.");
	//	return 0;
	//}

	//float color[4] = {
	//	material->diffuseColor.r,
	//	material->diffuseColor.g,
	//	material->diffuseColor.b,
	//	material->diffuseColor.a,
	//};

	//uint size = sizeof(color);

	//if (size == 0)
	//{
	//	return 0;
	//}

	//*buffer = new char[size];
	//char* cursor = *buffer;
	//uint bytes = 0;

	//// --- COLOR DATA ---
	//bytes = sizeof(color);
	//memcpy_s(cursor, size, color, bytes);
	//cursor += bytes;

	//// --- SAVING THE BUFFER ---
	//std::string path = std::string(MATERIALS_PATH) + std::to_string(material->GetUID()) + std::string(MATERIALS_EXTENSION);

	//written = App->fileSystem->Save(path.c_str(), *buffer, size);
	//if (written > 0)
	//{
	//	CONSOLE_LOG("[STATUS] Importer Materials: Successfully Saved Material { %s } to Library! Path: { %s }", material->GetAssetsFile(), path.c_str());
	//}
	//else
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Save Material { %s } to Library! Error: File System could not Write File.", material->GetAssetsFile());
	//}

	//return written;

	return true;
}

bool I_Material::Load(const char* buffer, Material* material)
{
	bool ret = true;

	//if (material == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Load Material from Library! Error: R_Material* was nullptr.");
	//	return false;
	//}
	//if (buffer == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Load Material { %s } from Library! Error: Buffer was nullptr.", material->GetAssetsFile());
	//	return false;
	//}

	//char* cursor = (char*)buffer;
	//uint bytes = 0;

	//float color[4];

	//bytes = sizeof(color);
	//memcpy_s(color, bytes, cursor, bytes);
	//cursor += bytes;

	//material->diffuseColor.Set(color[0], color[1], color[2], color[3]);

	//CONSOLE_LOG("[STATUS] Importer: Successfully Loaded Material { %s } from Library!", material->GetAssetsFile());

	return ret;
}