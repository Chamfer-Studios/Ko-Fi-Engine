#ifndef __I_MESH_H__
#define __I_MESH_H__

class Mesh;
class aiMesh;

class I_Mesh
{
public:
	I_Mesh();
	~I_Mesh();

	bool Import(const aiMesh* aiMesh, Mesh* mesh);
	bool Save(const Mesh* mesh, const char* buffer);
	bool Load(const char* path, Mesh* mesh);
};

#endif // !__I_MESH_H__