#ifndef __I_SHADER_H__
#define __I_SHADER_H__

#include <string>

class Shader;

enum class ShaderType
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

class I_Shader
{
public:
	I_Shader();
	~I_Shader();

	bool Import(const char* path, Shader* shader);
	unsigned int ImportShader(unsigned int type, const std::string& source);

	bool LoadUniforms(Shader* shader);
	bool CheckUniformName(std::string name);

	void DeleteShader(Shader* shader);
	void Recompile(Shader* shader);

	bool Save(const Shader* shader, const char* path);
	bool Load(const char* path, Shader* shader);
};

#endif // !__I_SHADER_H__