#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <string>
#include <vector>
#include "Globals.h"
#include "Color.h"

class Texture;

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

class Material
{
public:
	Material();
	~Material();

	Uniform* FindUniform(std::string name);
	void AddUniform(Uniform* uniform);

	inline const char* GetMaterialPath() const { return shaderPath.c_str(); }
	inline void SetMaterialPath(const char* name) { this->shaderPath = name; }

	inline const char* GetShaderPath() const { return shaderPath.c_str(); }
	inline void SetShaderPath(const char* name) { this->shaderPath = name; }

public:
	uint shaderProgramID;
	std::vector<Uniform*> uniforms;

	Color diffuseColor;

	std::string materialName;

private:
	std::string materialPath;
	std::string shaderPath;
};

#endif // !__MATERIAL_H__