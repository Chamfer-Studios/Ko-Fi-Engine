#include "Importer.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glew.h"
#include <gl/GL.h>
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Engine.h"
#include "SceneManager.h"
#include <fstream>

Importer* Importer::instance = nullptr;
Importer::Importer()
{
}
Importer::~Importer()
{
}

Importer* Importer::GetInstance() {
	
	if (instance == nullptr) {
		instance = new Importer();
	}
		return instance;
	
}

GameObject* Importer::ImportModel(const char* path)
{
	//Creating parent game object
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		if (scene->mNumMeshes > 1) {
			return GetMultipleMeshes(scene);
		}
		else if (scene->mNumMeshes == 1) {
			
			return GetOneMesh(scene);
		}

	}

	aiReleaseImport(scene);
	return nullptr;
}

bool Importer::SaveModel(const Mesh* mesh, const char* path)
{
	std::ofstream file;
	file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
	if (file.is_open()) {

		file.write((char*)mesh, 4 * sizeof(unsigned));


		file.write((char*)mesh->vertices, mesh->verticesSizeBytes);
		file.write((char*)mesh->normals, mesh->normalsSizeBytes);
		if (mesh->texCoordSizeBytes != 0) {
			file.write((char*)mesh->tex_coords, mesh->texCoordSizeBytes);
		}
		file.write((char*)mesh->indices, mesh->indicesSizeBytes);
		file.close();
		return true;
	}
	return false;
}

Mesh* Importer::LoadModel(const char* path)
{
	std::ifstream file;
	file.open(path, std::ios::binary);
	if (file.is_open()) {

		Mesh* mesh = new Mesh();
		file.read((char*)mesh, 4 * sizeof(unsigned));


		mesh->vertices = (float*)malloc(mesh->verticesSizeBytes);
		file.read((char*)mesh->vertices, mesh->verticesSizeBytes);

		mesh->normals = (float*)malloc(mesh->normalsSizeBytes);
		file.read((char*)mesh->normals, mesh->normalsSizeBytes);

		if (mesh->texCoordSizeBytes != 0) {
			mesh->tex_coords = (float*)malloc(mesh->texCoordSizeBytes);
			file.read((char*)mesh->tex_coords, mesh->texCoordSizeBytes);
		}
	

		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes);
		file.read((char*)mesh->indices, mesh->indicesSizeBytes);
		file.close();
		mesh->SetUpMeshBuffers();
		
		

		return mesh;
	}
	
	return nullptr;
}



GameObject* Importer::GetOneMesh(const aiScene* scene)
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
		ourMesh->tex_coords = (float*)malloc(ourMesh->texCoordSizeBytes);
		for (uint j = 0; j < aiMesh->mNumVertices; ++j)
		{
			ourMesh->tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
			ourMesh->tex_coords[j * 2 + 1] = 1.0f - aiMesh->mTextureCoords[0][j].y;
		}
	}

	ourMesh->SetUpMeshBuffers();

	if (scene->HasMaterials()) {
		texture = scene->mMaterials[aiMesh->mMaterialIndex];
		if (texture != nullptr) {
			aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texturePath);
			std::string newPath(texturePath.C_Str());
			if (newPath.size() > 0) {
				ourMesh->texture.texturePath = "Assets/Textures/" + newPath.substr(newPath.find_last_of('\\') + 1);
				ComponentMaterial* cMaterial = parent->CreateComponent<ComponentMaterial>();
				cMaterial->AddTextures(ourMesh->texture);
				if (newPath.c_str() != nullptr) {
					cMaterial->LoadTexture(ourMesh->texture.texturePath.c_str());

				}
				else {
					cMaterial->LoadDefaultTexture(ourMesh->texture.textureID);
				}

			}
		}
	}
	//materialComponent->AddTextures(ourMesh->texture);
	ComponentMesh* cMesh = parent->CreateComponent<ComponentMesh>();
	cMesh->SetMesh(ourMesh);
	return parent;
}

GameObject* Importer::GetMultipleMeshes(const aiScene* scene)
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
		ourMesh->verticesSizeBytes = aiMesh->mNumVertices * sizeof(float)*3;
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
			ourMesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float)*3;
			ourMesh->normals = (float*)malloc(ourMesh->normalsSizeBytes);
			memcpy(ourMesh->normals, aiMesh->mNormals,ourMesh->normalsSizeBytes);
		}

		// Texture coordinates
		if (aiMesh->HasTextureCoords(0))
		{
			ourMesh->texCoordSizeBytes = aiMesh->mNumVertices * sizeof(float)*2;
			ourMesh->tex_coords = (float*)malloc(ourMesh->texCoordSizeBytes);
			for (uint j = 0; j < aiMesh->mNumVertices; ++j)
			{
				ourMesh->tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
				ourMesh->tex_coords[j * 2 + 1] = /*1.0f - */aiMesh->mTextureCoords[0][j].y;
			}
		}
		else
			ourMesh->tex_coords = 0;

		ourMesh->SetUpMeshBuffers();

		if (scene->HasMaterials()) {
			texture = scene->mMaterials[aiMesh->mMaterialIndex];
			if (texture != nullptr) {
				aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texturePath);
				std::string newPath(texturePath.C_Str());
				if (newPath.size() > 0) {
					ourMesh->texture.texturePath = "Assets/Textures/" + newPath.substr(newPath.find_last_of('\\')+1);
					ComponentMaterial* cMaterial = child->CreateComponent<ComponentMaterial>();
					cMaterial->AddTextures(ourMesh->texture);
					if (newPath.c_str() != nullptr) {
						cMaterial->LoadTexture(ourMesh->texture.texturePath.c_str());

					}
					else {
						cMaterial->LoadDefaultTexture(ourMesh->texture.textureID);
					}

				}
			}
		}
		//materialComponent->AddTextures(ourMesh->texture);
		ComponentMesh* cMesh = child->CreateComponent<ComponentMesh>();
		cMesh->SetMesh(ourMesh);
		child = nullptr;
	}
	return parent;
}
