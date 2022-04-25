#ifndef __I_MESH_H__
#define __I_MESH_H__

#include "Assimp.h"
#include "MathGeoLib/Math/float4x4.h"
#include <map>

class R_Mesh;
class KoFiEngine;
typedef unsigned int uint;

class I_Mesh
{
public:
	I_Mesh(KoFiEngine* engine);
	~I_Mesh();

	bool Import(const aiMesh* aiMesh, R_Mesh* mesh, const aiScene* assimpScene = nullptr);
	bool Save(const R_Mesh* mesh, const char* path);
	bool Load(const char* path, R_Mesh* mesh);
	// Util functions for this importer
	int GetBoneId(const aiBone* pBone, std::map<std::string, uint>& boneNameToIndexMap);
	float4x4 aiMatrix2Float4x4(aiMatrix4x4& aiOffsetMat)
	{
		return float4x4(aiOffsetMat.a1, aiOffsetMat.a2, aiOffsetMat.a3, aiOffsetMat.a4,
						aiOffsetMat.b1, aiOffsetMat.b2, aiOffsetMat.b3, aiOffsetMat.b4,
						aiOffsetMat.c1, aiOffsetMat.c2, aiOffsetMat.c3, aiOffsetMat.c4,
						aiOffsetMat.d1, aiOffsetMat.d2, aiOffsetMat.d3, aiOffsetMat.d4);
	}

private:
	KoFiEngine* engine = nullptr;
};

#endif // !__I_MESH_H__