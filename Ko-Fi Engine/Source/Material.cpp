#include "Material.h"
#include "FSDefs.h"
#include "Color.h"
#include "Texture.h"
#include <glew.h>

Material::Material() :
	shaderProgramID(0),
	materialPath(MATERIALS_DIR + std::string("default") + MATERIAL_EXTENSION),
	shaderPath(SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION)
{
}

Material::~Material()
{
	glDeleteProgram(shaderProgramID);

	uniforms.clear();

	shaderPath.clear();
	shaderPath.shrink_to_fit();
}

Uniform* Material::FindUniform(std::string name)
{
	for (Uniform* uniform : uniforms)
	{
		if (uniform->name == name)
		{
			return uniform;
		}
	}
	return nullptr;
}

void Material::AddUniform(Uniform* uniform)
{
	uniforms.push_back(uniform);
}