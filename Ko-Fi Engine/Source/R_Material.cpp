#include "R_Material.h"
#include "FSDefs.h"
#include "Color.h"
#include "R_Texture.h"
#include <glew.h>

R_Material::R_Material() :
	shaderProgramID(0),
	Resource(ResourceType::MATERIAL)
{
	std::string path = ASSETS_SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION;
	SetAssetPath(path.c_str());
}

R_Material::~R_Material()
{
	glDeleteProgram(shaderProgramID);

	uniforms.clear();
	uniforms.shrink_to_fit();
}

Uniform* R_Material::FindUniform(std::string name)
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

void R_Material::AddUniform(Uniform* uniform)
{
	uniforms.push_back(uniform);
}