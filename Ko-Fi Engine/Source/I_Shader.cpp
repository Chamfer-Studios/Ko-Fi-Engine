#include "I_Shader.h"
#include "Shader.h"

I_Shader::I_Shader()
{
}

I_Shader::~I_Shader()
{
}

bool I_Shader::Import(const aiMesh* aiMaterial, Shader* shader)
{
	return true;
}

bool I_Shader::Save(const Shader* shader, const char* path)
{
	return true;
}

bool I_Shader::Load(const char* path, Shader* shader)
{
	return true;
}