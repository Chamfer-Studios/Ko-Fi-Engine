#ifndef __I_TEXTURE__
#define __I_TEXTURE__

namespace Importer
{
	namespace Textures
	{
		bool Import(const aiMesh* aiMaterial, Mesh* mesh);
		bool Save(const Mesh* mesh, const char* path);
		bool Load(const char* path, Mesh* mesh);
	}
}

#endif // !__I_TEXTURE__