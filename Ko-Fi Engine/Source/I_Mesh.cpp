#include "I_Mesh.h"
#include "Globals.h"
#include "Log.h"

#include "Mesh.h"

#include <fstream>

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
		return false;
	}
	if (aiMesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Mesh! Error: aiMesh* was nullptr.");
		return false;
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
		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes); // Assume each face is a triangle

		for (uint i = 0; i < aiMesh->mNumFaces; ++i)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
				CONSOLE_LOG("[WARNING] Geometry face %u with != 3 indices!", i);
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
			mesh->texCoords[j * 2 + 1] = 1.0f - aiMesh->mTextureCoords[0][j].y;
		}

		CONSOLE_LOG("[STATUS] Imported %u texture coordinates!", aiMesh->mNumVertices * 2);
	}
	else
		mesh->texCoords = 0;

	mesh->SetUpMeshBuffers();

	return true;
}

bool I_Mesh::Save(const Mesh* mesh, const char* path)
{
	std::ofstream file;
	file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
	if (file.is_open())
	{
		file.write((char*)mesh, 4 * sizeof(unsigned));

		file.write((char*)mesh->indices, mesh->indicesSizeBytes);

		file.write((char*)mesh->vertices, mesh->verticesSizeBytes);

		file.write((char*)mesh->normals, mesh->normalsSizeBytes);

		if (mesh->texCoordSizeBytes != 0)
			file.write((char*)mesh->texCoords, mesh->texCoordSizeBytes);

		file.close();

		return true;
	}

	return false;
}

bool I_Mesh::Load(const char* path, Mesh* mesh)
{
	std::ifstream file;
	file.open(path, std::ios::binary);
	if (file.is_open())
	{
		file.read((char*)mesh, 4 * sizeof(unsigned));

		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes);
		file.read((char*)mesh->indices, mesh->indicesSizeBytes);

		mesh->vertices = (float*)malloc(mesh->verticesSizeBytes);
		file.read((char*)mesh->vertices, mesh->verticesSizeBytes);

		mesh->normals = (float*)malloc(mesh->normalsSizeBytes);
		file.read((char*)mesh->normals, mesh->normalsSizeBytes);

		if (mesh->texCoordSizeBytes != 0)
		{
			mesh->texCoords = (float*)malloc(mesh->texCoordSizeBytes);
			file.read((char*)mesh->texCoords, mesh->texCoordSizeBytes);
		}

		file.close();

		mesh->SetUpMeshBuffers();

		return true;
	}
	return false;
}