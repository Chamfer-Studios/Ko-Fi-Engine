#include "I_Mesh.h"
#include "Mesh.h"
#include <fstream>

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