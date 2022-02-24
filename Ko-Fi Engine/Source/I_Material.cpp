#include "I_Material.h"
#include "Material.h"

I_Material::I_Material()
{
}

I_Material::~I_Material()
{
}

bool I_Material::Import(const aiMaterial* aiMaterial, Material* material)
{
	return true;
}

bool I_Material::Save(const Material* material)
{
	return true;
}

bool I_Material::Load(const char* path, Material* material)
{
	return true;
}