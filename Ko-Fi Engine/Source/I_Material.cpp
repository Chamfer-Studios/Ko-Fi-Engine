#include "I_Material.h"

bool Importer::Materials::Import(const aiMaterial* aiMaterial, Material* material)
{
	return true;
}

bool Importer::Materials::Save(const Material* material)
{
	return true;
}

bool Importer::Materials::Load(const char* path, Mesh* mesh)
{
	return true;
}