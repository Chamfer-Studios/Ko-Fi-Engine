#ifndef __MATERIAL_H__
#define __MATERIAL_H__

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

	Uniform* FindUniform(std::string name);
	void AddUniform(Uniform* uniform);

	inline const char* GetShaderPath() const { return GetAssetPath(); }
	inline void SetShaderPath(const char* name) { SetAssetPath(name); }

public:
	uint shaderProgramID =0;
	std::vector<Uniform*> uniforms;

	Color diffuseColor;
};

#endif // !__MATERIAL_H__