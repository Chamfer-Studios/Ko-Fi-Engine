#ifndef __I_MATERIAL_H__
#define __I_MATERIAL_H__

class Material;
class aiMaterial;

class I_Material
{
public:
	I_Material();
	~I_Material();

	bool Import(const aiMaterial* aiMaterial, Material* material);
	bool Save(const Material* material);
	bool Load(Material* material);
};

#endif // !__I_MATERIAL_H__