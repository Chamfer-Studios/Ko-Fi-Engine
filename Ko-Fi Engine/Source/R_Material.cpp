#include "R_Material.h"
#include "FSDefs.h"
#include "Color.h"
#include "R_Texture.h"
#include <glew.h>

R_Material::R_Material() :
	shaderProgramID(SHADERID_DEFAULT),
	Resource(ResourceType::MATERIAL)
{
}

R_Material::~R_Material()
{
	glDeleteProgram(shaderProgramID);

	for (std::vector<Uniform*>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
	{
		(*it)->name.clear();
		(*it)->name.shrink_to_fit();
		delete *it;
	}
	uniforms.clear();
	uniforms.shrink_to_fit();
}

bool R_Material::SaveMeta(Json& json) const
{
	json["contained_resources"].array();
	return true;
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