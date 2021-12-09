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
Importer::Importer(KoFiEngine* engine)
{
	this->engine = engine;
}
Importer::~Importer()
{
}
void Importer::ImportModel(const char* path)
{
	//Creating parent game object
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		if (scene->mNumMeshes > 1) {
			GetMultipleMeshes(scene);
		}
		else if (scene->mNumMeshes == 1) {
			GetOneMesh(scene);
		}

	}

	aiReleaseImport(scene);
}

void Importer::GetOneMesh(const aiScene* scene)
{
	//TODO: ADD MATERIAL
	//materialComponent = new ComponentMaterial(this->owner);
	GameObject* parent = engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();

	Mesh* ourMesh = new Mesh();
	aiMesh* aiMesh = scene->mMeshes[0];

	// Positions
	ourMesh->num_vertices = aiMesh->mNumVertices;
	ourMesh->vertices = new float[ourMesh->num_vertices * 3];
	memcpy(ourMesh->vertices, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertices * 3); // &vertices[0]

	// Faces
	if (aiMesh->HasFaces())
	{
		ourMesh->num_indices = aiMesh->mNumFaces * 3;
		ourMesh->indices = new uint[ourMesh->num_indices]; // assume each face is a triangle
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
		ourMesh->num_normals = aiMesh->mNumVertices;
		ourMesh->normals = new float[ourMesh->num_normals * 3];
		memcpy(ourMesh->normals, aiMesh->mNormals, sizeof(float) * ourMesh->num_normals * 3);
	}

	// Texture coordinates
	if (aiMesh->HasTextureCoords(0))
	{
		ourMesh->num_tex_coords = aiMesh->mNumVertices;
		ourMesh->tex_coords = new float[ourMesh->num_tex_coords * 2];
		for (uint j = 0; j < ourMesh->num_tex_coords; ++j)
		{
			ourMesh->tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
			ourMesh->tex_coords[j * 2 + 1] = /*1.0f - */aiMesh->mTextureCoords[0][j].y;
		}
	}
	else
		ourMesh->tex_coords = 0;

	ourMesh->SetUpMeshBuffers();
	//materialComponent->AddTextures(ourMesh->texture);
	ComponentMesh* cMesh = parent->CreateComponent<ComponentMesh>();
	cMesh->SetMesh(ourMesh);
}

void Importer::GetMultipleMeshes(const aiScene* scene)
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
		ourMesh->num_vertices = aiMesh->mNumVertices;
		ourMesh->vertices = new float[ourMesh->num_vertices * 3];
		memcpy(ourMesh->vertices, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertices * 3); // &vertices[0]

		// Faces
		if (aiMesh->HasFaces())
		{
			ourMesh->num_indices = aiMesh->mNumFaces * 3;
			ourMesh->indices = new uint[ourMesh->num_indices]; // assume each face is a triangle
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
			ourMesh->num_normals = aiMesh->mNumVertices;
			ourMesh->normals = new float[ourMesh->num_normals * 3];
			memcpy(ourMesh->normals, aiMesh->mNormals, sizeof(float) * ourMesh->num_normals * 3);
		}

		// Texture coordinates
		if (aiMesh->HasTextureCoords(0))
		{
			ourMesh->num_tex_coords = aiMesh->mNumVertices;
			ourMesh->tex_coords = new float[ourMesh->num_tex_coords * 2];
			for (uint j = 0; j < ourMesh->num_tex_coords; ++j)
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
}