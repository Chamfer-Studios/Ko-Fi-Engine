#include "I_Mesh.h"
#include "Mesh.h"
#include <fstream>

I_Mesh::I_Mesh()
{
}

I_Mesh::~I_Mesh()
{
}

bool I_Mesh::Import(const aiMesh* aiMaterial, Mesh* mesh)
{
	return true;
}

bool I_Mesh::Save(const Mesh* mesh, const char* path)
{
	return true;
}

bool I_Mesh::Load(const char* path, Mesh* mesh)
{
	return true;
}

bool Importer::Scenes::ImportOne(const aiScene* scene, )
{
	aiMaterial* texture = nullptr;
	aiString texturePath;
	GameObject* parent = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
	Mesh* ourMesh = new Mesh();
	aiMesh* aiMesh = scene->mMeshes[0];

	// Positions
	ourMesh->verticesSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
	ourMesh->vertices = (float*)malloc(ourMesh->verticesSizeBytes);
	memcpy(ourMesh->vertices, aiMesh->mVertices, ourMesh->verticesSizeBytes); // &vertices[0]

	// Faces
	if (aiMesh->HasFaces())
	{
		ourMesh->indicesSizeBytes = aiMesh->mNumFaces * sizeof(uint) * 3;
		ourMesh->indices = (uint*)malloc(ourMesh->indicesSizeBytes); // assume each face is a triangle
		for (uint i = 0; i < aiMesh->mNumFaces; ++i)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
			{
				/*                       CONSOLE_LOG("WARNING, geometry face with != 3 indices!");
									   appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
			}
			else
				memcpy(&ourMesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	// Loading mesh normals data
	if (aiMesh->HasNormals())
	{
		ourMesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		ourMesh->normals = (float*)malloc(ourMesh->normalsSizeBytes);
		memcpy(ourMesh->normals, aiMesh->mNormals, ourMesh->normalsSizeBytes);
	}

	// Texture coordinates
	if (aiMesh->HasTextureCoords(0))
	{
		ourMesh->texCoordSizeBytes = aiMesh->mNumVertices * sizeof(float) * 2;
		ourMesh->texCoords = (float*)malloc(ourMesh->texCoordSizeBytes);
		for (uint j = 0; j < aiMesh->mNumVertices; ++j)
		{
			ourMesh->texCoords[j * 2] = aiMesh->mTextureCoords[0][j].x;
			ourMesh->texCoords[j * 2 + 1] = 1.0f - aiMesh->mTextureCoords[0][j].y;
		}
	}

	ourMesh->SetUpMeshBuffers();

	if (scene->HasMaterials()) {
		texture = scene->mMaterials[aiMesh->mMaterialIndex];
		if (texture != nullptr) {

			aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texturePath);
			std::string newPath(texturePath.C_Str());
			ComponentMaterial* cMaterial = parent->CreateComponent<ComponentMaterial>();

			if (newPath.size() > 0) {
				std::string base_filename = newPath.substr(newPath.find_last_of("/\\") + 1);
				std::string::size_type const p(base_filename.find_last_of('.'));
				std::string filenameWithoutExtension = base_filename.substr(0, p);
				std::string materialPath = "Assets/Materials/" + filenameWithoutExtension + ".milk";
				std::string texturePath = "Assets/Textures/" + newPath.substr(newPath.find_last_of('\\') + 1);
				if (newPath.c_str() != nullptr) {
					engine->GetFileSystem()->CreateMaterial(materialPath.c_str(), filenameWithoutExtension.c_str(), texturePath.c_str());
					cMaterial->LoadMaterial(materialPath.c_str());
				}

			}
		}
	}
	ComponentMesh* cMesh = parent->CreateComponent<ComponentMesh>();
	cMesh->SetMesh(ourMesh);
	return parent;
}


bool Importer::Scenes::ImportMultiple(const aiScene* scene)
{
	GameObject* parent = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
	aiMaterial* texture = nullptr;
	aiString texturePath;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		GameObject* child = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
		parent->AttachChild(child);
		Mesh* ourMesh = new Mesh();
		aiMesh* aiMesh = scene->mMeshes[i];

		// Positions
		ourMesh->verticesSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		ourMesh->vertices = (float*)malloc(ourMesh->verticesSizeBytes);
		memcpy(ourMesh->vertices, aiMesh->mVertices, ourMesh->verticesSizeBytes); // &vertices[0]

		// Faces
		if (aiMesh->HasFaces())
		{
			ourMesh->indicesSizeBytes = aiMesh->mNumFaces * sizeof(uint) * 3;
			ourMesh->indices = (uint*)malloc(ourMesh->indicesSizeBytes); // assume each face is a triangle
			for (uint i = 0; i < aiMesh->mNumFaces; ++i)
			{
				if (aiMesh->mFaces[i].mNumIndices != 3)
				{
					/*                       CONSOLE_LOG("WARNING, geometry face with != 3 indices!");
										   appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
				}
				else
					memcpy(&ourMesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}

		// Loading mesh normals data
		if (aiMesh->HasNormals())
		{
			ourMesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
			ourMesh->normals = (float*)malloc(ourMesh->normalsSizeBytes);
			memcpy(ourMesh->normals, aiMesh->mNormals, ourMesh->normalsSizeBytes);
		}

		// Texture coordinates
		if (aiMesh->HasTextureCoords(0))
		{
			ourMesh->texCoordSizeBytes = aiMesh->mNumVertices * sizeof(float) * 2;
			ourMesh->texCoords = (float*)malloc(ourMesh->texCoordSizeBytes);
			for (uint j = 0; j < aiMesh->mNumVertices; ++j)
			{
				ourMesh->texCoords[j * 2] = aiMesh->mTextureCoords[0][j].x;
				ourMesh->texCoords[j * 2 + 1] = /*1.0f - */aiMesh->mTextureCoords[0][j].y;
			}
		}
		else
			ourMesh->texCoords = 0;

		ourMesh->SetUpMeshBuffers();

		if (scene->HasMaterials()) {
			texture = scene->mMaterials[aiMesh->mMaterialIndex];
			if (texture != nullptr) {

				aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texturePath);
				std::string newPath(texturePath.C_Str());
				ComponentMaterial* cMaterial = child->CreateComponent<ComponentMaterial>();
				if (newPath.size() > 0) {
					std::string base_filename = newPath.substr(newPath.find_last_of("/\\") + 1);
					std::string::size_type const p(base_filename.find_last_of('.'));
					std::string filenameWithoutExtension = base_filename.substr(0, p);
					std::string materialPath = "Assets/Materials/" + filenameWithoutExtension + ".milk";
					std::string texturePath = "Assets/Textures/" + newPath.substr(newPath.find_last_of('\\') + 1);
					if (newPath.c_str() != nullptr) {
						engine->GetFileSystem()->CreateMaterial(materialPath.c_str(), filenameWithoutExtension.c_str(), texturePath.c_str());
						cMaterial->LoadMaterial(materialPath.c_str());
					}

				}
			}
		}
		ComponentMesh* cMesh = child->CreateComponent<ComponentMesh>();
		cMesh->SetMesh(ourMesh);
		child = nullptr;
	}
	return parent;
}



bool Importer::Meshes::Import(const aiMesh* aiMaterial, Mesh* mesh)
{
	return true;
}

bool Importer::Meshes::Save(const Mesh* mesh, const char* path)
{
	std::ofstream file;
	file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
	if (file.is_open()) {

		file.write((char*)mesh, 4 * sizeof(unsigned));


		file.write((char*)mesh->vertices, mesh->verticesSizeBytes);
		file.write((char*)mesh->normals, mesh->normalsSizeBytes);
		if (mesh->texCoordSizeBytes != 0) {
			file.write((char*)mesh->texCoords, mesh->texCoordSizeBytes);
		}
		file.write((char*)mesh->indices, mesh->indicesSizeBytes);
		file.close();
		return true;
	}
	return false;
}

bool Importer::Meshes::Load(const char* path, Mesh* mesh)
{
	return true;
}