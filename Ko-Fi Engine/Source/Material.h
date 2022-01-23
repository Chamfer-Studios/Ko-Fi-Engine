#pragma once
#include "MathGeoLib/Math/float4.h"
#include "Texture.h"
#include <string>
struct Uniform {
	Uniform(std::string name, uint type) : name(name),type(type){};
	uint type;
	std::string name;

};
template<class T>

struct UniformT : public Uniform {
	UniformT(std::string name,uint type, T value) : Uniform(name,type),value(value){}
	T value;
};
class Material {
public:
	Material() {}
	~Material() {}
	Uniform* FindUniform(std::string name) {
		for (Uniform* uniform : uniforms) {
			if (uniform->name == name) {
				return uniform;
			}
		}
		return nullptr;
	}
	void AddUniform(Uniform* uniform) {
		uniforms.push_back(uniform);
	}
public:
	std::string materialName;
	std::string materialPath;
	//Crear un vector de uniforms con id, type y valor y guardarlos ahi para luego sacar los datos
	std::vector<Uniform*> uniforms;
	std::vector<Texture> textures;
};