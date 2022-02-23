#ifndef __I_MESH__
#define __I_MESH__

class aiMesh;
class Mesh;

namespace Importer
{
	namespace Meshes
	{
		bool Import(const aiMesh* aiMaterial, Mesh* mesh);
		bool Save(const Mesh* mesh, const char* path);
		bool Load(const char* path, Mesh* mesh);
	}
}

#endif // !__I_MESH__