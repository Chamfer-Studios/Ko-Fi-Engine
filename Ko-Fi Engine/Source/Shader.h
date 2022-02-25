#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <vector>
#include "Globals.h"

struct Uniform {
	Uniform(std::string name, uint type) : name(name), type(type) {};
	uint type;
	std::string name;

};
template<class T>

struct UniformT : public Uniform {
	UniformT(std::string name, uint type, T value) : Uniform(name, type), value(value) {}
	T value;
};

class Shader
{
public:
	Shader();
	~Shader();

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
	std::string vertexSource;
	std::string fragmentSource;
	uint materialShader = 0;
	std::vector<Uniform*> uniforms;
}

#endif // !__SHADER_H__