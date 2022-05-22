#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#define SHADERID_DEFAULT 4294967295

#include "Globals.h"
#include "Color.h"
#include "Resource.h"

struct Uniform
{
	Uniform(std::string name, uint type) : name(name), type(type) {};
	uint type;
	std::string name;
};

template<class T>
struct UniformT : public Uniform
{
	UniformT(std::string name, uint type, T value) : Uniform(name, type), value(value) {}
	T value;
};

class R_Material : public Resource
{
public:
	R_Material();
	~R_Material();

	bool SaveMeta(Json& json) const override;

	Uniform* FindUniform(std::string name);
	void AddUniform(Uniform* uniform);

public:
	uint shaderProgramID = SHADERID_DEFAULT;
	std::vector<Uniform*> uniforms;

	Color diffuseColor;
};

#endif // !__MATERIAL_H__