#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <vector>
#include "Globals.h"
#include "glew.h"
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"

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

class Shader
{
public:
	Shader();
	~Shader();

	Uniform* FindUniform(std::string name);

	void AddUniform(Uniform* uniform);

	void UseShader();

	const char* GetShaderPath() const;
	void SetShaderPath(const char* name);

	// Functionality to be able to save/modify data of Shader
	void SetUniformMatrix4(std::string name, GLfloat* value);

	void SetUniformVec2f(std::string name, GLfloat* value);
	void SetUniformVec3f(std::string name, GLfloat* value);
	void SetUniformVec4f(std::string name, GLfloat* value);

	void SetUniformVec2i(std::string name, GLint* value);
	void SetUniformVec3i(std::string name, GLint* value);
	void SetUniformVec4i(std::string name, GLint* value);

	void SetUniform1f(std::string name, GLfloat value);
	void SetUniform1i(std::string name, GLint value);
	void SetBool(std::string name, bool value);

public:
	uint shaderProgramID = 0;

	std::vector<Uniform*> uniforms;

private:
	std::string path;
};

#endif // !__SHADER_H__