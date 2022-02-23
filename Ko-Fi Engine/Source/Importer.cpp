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
#include "FileSystem.h"
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

		if (mesh->texCoordSizeBytes != 0)
		{
			mesh->texCoords = (float*)malloc(mesh->texCoordSizeBytes);
			file.read((char*)mesh->texCoords, mesh->texCoordSizeBytes);
		}

		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes);
		file.read((char*)mesh->indices, mesh->indicesSizeBytes);
		file.close();
		mesh->SetUpMeshBuffers();

		return mesh;
	}
	
	return nullptr;
}




