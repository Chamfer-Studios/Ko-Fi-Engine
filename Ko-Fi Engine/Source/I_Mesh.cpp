#include "I_Mesh.h"
#include "Mesh.h"
#include <fstream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Log.h"

I_Mesh::I_Mesh()
{
}

I_Mesh::~I_Mesh()
{
}

bool I_Mesh::Import(const aiMesh* aiMesh, Mesh* mesh)
{
	if (mesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Mesh! Error: R_Mesh* was nullptr.");
		return;
	}
	if (aiMesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Mesh { %s }! Error: aiMesh* was nullptr.", mesh->GetAssetFile());
		return;
	}

	// Loading the data from the mesh into the corresponding vectors
	if (aiMesh->HasPositions())
	{
		// Positions
		mesh->verticesSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		mesh->vertices = (float*)malloc(mesh->verticesSizeBytes);
		memcpy(mesh->vertices, aiMesh->mVertices, mesh->verticesSizeBytes); // &vertices[0]

		CONSOLE_LOG("[STATUS] Imported %u position vertices!", aiMesh->mNumVertices * 3);
	}

	if (aiMesh->HasFaces())
	{
		mesh->indicesSizeBytes = aiMesh->mNumFaces * sizeof(uint) * 3;
		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes); // assume each face is a triangle
		for (uint i = 0; i < aiMesh->mNumFaces; ++i)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
			{
				CONSOLE_LOG("[WARNING] Geometry face %u with != 3 indices!", i);
				/*                       CONSOLE_LOG("WARNING, geometry face with != 3 indices!");
									   appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
			}
			else
				memcpy(&mesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	if (aiMesh->HasNormals())
	{
		mesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		mesh->normals = (float*)malloc(mesh->normalsSizeBytes);
		memcpy(mesh->normals, aiMesh->mNormals, mesh->normalsSizeBytes);

		CONSOLE_LOG("[STATUS] Imported %u normals!", aiMesh->mNumVertices * 3);
	}

	if (aiMesh->HasTextureCoords(0))
	{
		mesh->texCoordSizeBytes = aiMesh->mNumVertices * sizeof(float) * 2;
		mesh->texCoords = (float*)malloc(mesh->texCoordSizeBytes);
		for (uint j = 0; j < aiMesh->mNumVertices; ++j)
		{
			mesh->texCoords[j * 2] = aiMesh->mTextureCoords[0][j].x;
			mesh->texCoords[j * 2 + 1] = aiMesh->mTextureCoords[0][j].y;
		}
	}
	else
		mesh->texCoords = 0;

	CONSOLE_LOG("[STATUS] Imported %u texture coordinates!", aiMesh->mNumVertices * 2);

	mesh->SetUpMeshBuffers();

	//if (scene->HasMaterials()) {
	//	texture = scene->mMaterials[aiMesh->mMaterialIndex];
	//	if (texture != nullptr) {

	//		aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texturePath);
	//		std::string newPath(texturePath.C_Str());
	//		ComponentMaterial* cMaterial = child->CreateComponent<ComponentMaterial>();
	//		if (newPath.size() > 0) {
	//			std::string base_filename = newPath.substr(newPath.find_last_of("/\\") + 1);
	//			std::string::size_type const p(base_filename.find_last_of('.'));
	//			std::string filenameWithoutExtension = base_filename.substr(0, p);
	//			std::string materialPath = "Assets/Materials/" + filenameWithoutExtension + ".milk";
	//			std::string texturePath = "Assets/Textures/" + newPath.substr(newPath.find_last_of('\\') + 1);
	//			if (newPath.c_str() != nullptr) {
	//				engine->GetFileSystem()->CreateMaterial(materialPath.c_str(), filenameWithoutExtension.c_str(), texturePath.c_str());
	//				cMaterial->LoadMaterial(materialPath.c_str());
	//			}

	//		}
	//	}
	//}

	return true;
}

bool I_Mesh::Save(const Mesh* mesh, const char* path)
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

bool I_Mesh::Load(const char* path, Mesh* mesh)
{

	std::ifstream file;
	file.open(path, std::ios::binary);
	if (file.is_open()) {

		mesh = new Mesh();
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

		return true;
	}

	return false;
}