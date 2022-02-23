#ifndef __I_SHADER__
#define __I_SHADER__

namespace Importer
{
	namespace Shaders
	{
		bool Import(const aiMesh* aiMaterial, Mesh* mesh);
		bool Save(const Mesh* mesh, const char* path);
		bool Load(const char* path, Mesh* mesh);
	}
}

#endif // !__I_SHADER__