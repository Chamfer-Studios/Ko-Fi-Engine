#ifndef __I_MESH_H__
#define __I_MESH_H__

class aiMesh;
class Mesh;

class I_Mesh
{
public:
	I_Mesh();
	~I_Mesh();

	bool ImportOne(const aiScene* aiScene, Scene* scene);
	bool ImportMultiple(const aiScene* aiScene, Scene* scene);
	bool Import(const aiMesh* aiMaterial, Mesh* mesh);
	bool Save(const Mesh* mesh, const char* path);
	bool Load(const char* path, Mesh* mesh);
};

#endif // !__I_MESH_H__