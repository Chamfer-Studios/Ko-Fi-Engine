#ifndef __I_MATERIAL__
#define __I_MATERIAL__

class Material;
class aiMaterial;
class Mesh;

namespace Importer
{
	namespace Materials
	{
		bool Import(const aiMaterial* aiMaterial, Material* material);
		bool Save(const Material* material);
		bool Load(const char* path, Mesh* mesh);
	}
}

#endif // !__I_MATERIAL__