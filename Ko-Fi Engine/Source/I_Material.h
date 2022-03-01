#ifndef __I_MATERIAL_H__
#define __I_MATERIAL_H__

#include <string>

class Material;
class aiMaterial;

enum class ShaderType
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

class I_Material
{
public:
	I_Material();
	~I_Material();

	bool Import(const aiMaterial* aiMaterial, Material* material);

	unsigned int ImportShader(unsigned int type, const std::string& source);
	bool LoadUniforms(Material* material);
	bool CheckUniformName(std::string name);

	bool Save(const Material* material);
	bool Load(Material* material);
};

#endif // !__I_MATERIAL_H__