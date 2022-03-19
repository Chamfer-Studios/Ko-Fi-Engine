#ifndef __I_MESH_H__
#define __I_MESH_H__

#include "Assimp.h"
#include <map>
class Mesh;
typedef unsigned int uint;
class I_Mesh
{
public:
	I_Mesh();
	~I_Mesh();

	bool Import(const aiMesh* aiMesh, Mesh* mesh);
	bool Save(const Mesh* mesh, const char* path);
	bool Load(const char* path, Mesh* mesh);
	//Util functions for this importer
	int GetBoneId(const aiBone* pBone, std::map<std::string, uint>& boneNameToIndexMap);
};

#endif // !__I_MESH_H__