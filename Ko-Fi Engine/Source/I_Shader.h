#ifndef __I_SHADER_H__
#define __I_SHADER_H__

class Shader;

class I_Shader
{
public:
	I_Shader();
	~I_Shader();

	bool Import(const aiMesh* aiMaterial, Shader* shader);
	bool Save(const Shader* shader, const char* path);
	bool Load(const char* path, Shader* shader);
};

#endif // !__I_SHADER_H__