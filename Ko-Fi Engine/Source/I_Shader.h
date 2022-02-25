#ifndef __I_SHADER_H__
#define __I_SHADER_H__

#include <string>

class Shader;

class I_Shader
{
public:
	I_Shader();
	~I_Shader();

	bool Import(const char* path, Shader* shader);
	bool Save(const Shader* shader, const char* path);
	bool Load(const char* path, Shader* shader);

	void DeleteShader(Shader* shader);
	void Recompile(Shader* shader);
	bool LoadUniforms(Shader* shader);
	unsigned int CompileShader(unsigned int type, const std::string& source);
};

#endif // !__I_SHADER_H__