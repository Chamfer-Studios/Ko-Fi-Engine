#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <map>
#include <string>

#include "MathGeoLib/Geometry/AABB.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/TransformOps.h"

#include "Assimp.h"
#include "par_shapes.h"
#include "Globals.h"
#include "Log.h"

#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2
#define BONE_LOCATION 3
#define WEIGHT_LOCATION 4

#define MAX_NUM_BONES_PER_VERTEX 4

class GameObject;
class aiBone;

enum class Shape
{
	NONE,
	CUBE,
	SPHERE,
	CYLINDER,
	TORUS,
	PLANE,
	CONE
};

struct VertexBoneData
{
	uint boneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
	float weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
	VertexBoneData() {}
	void AddBoneData(uint boneID, float weight)
	{
		for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(boneIDs); i++)
		{
			if (weights[i] == 0.0)
			{
				boneIDs[i] = boneID;
				weights[i] = weight;
				CONSOLE_LOG("bone %d weight %f index %i\n",boneID,weight,i);
				return;
			}
		}
		assert(0);
	}
};

struct BoneInfo
{
	float4x4 offsetMatrix;
	float4x4 finalTransformation;

	BoneInfo(const float4x4& offset)
	{
		offsetMatrix = offset;
		finalTransformation = float4x4::zero;
	}
};

class Mesh
{
public:
	Mesh(Shape shape = Shape::NONE);
	~Mesh();

	// Mesh Functions
	void SetUpMeshBuffers();

	void Draw();
	void DebugDraw();

	inline void SetVertexNormals(bool vertex) { drawVertexNormals = vertex; }
	inline bool GetVertexNormals() const { return drawVertexNormals; }

	inline void SetFaceNormals(bool faces) { drawFaceNormals = faces; }
	inline bool GetFaceNormals() const { return drawFaceNormals; }

	void GetBoneTransforms(float timeInSeconds, std::vector<float4x4>& transforms, GameObject* gameObject);
	void ReadNodeHeirarchy(float animationTimeTicks, const aiNode* pNode, const float4x4& parentTransform);

	void SetRootNode(const aiScene* assimpScene);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string nodeName);

	uint FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	uint FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	uint FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	float4x4 InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
	float4x4 InitRotateTransform(const aiQuaternion& quat);
	float4x4 InitTranslationTransform(float x, float y, float z);
	float4x4 aiMatrix3x32Float4x4(aiMatrix3x3 assimpMatrix);
	float* GetTransformedVertices(float4x4 transform);

	static Mesh* MeshUnion(std::vector<Mesh*> meshes, std::vector<float4x4> transformations);

	// Size in Bytes
	unsigned verticesSizeBytes = 0;
	unsigned normalsSizeBytes = 0;
	unsigned texCoordSizeBytes = 0;
	unsigned indicesSizeBytes = 0;

	// Indices
	uint idIndex = 0; // index in VRAM
	uint* indices = nullptr;

	// Vertices
	uint idVertex = 0; // unique vertex in VRAM
	float* vertices = nullptr;

	// Normals
	uint idNormal = 0;
	float* normals = nullptr;

	// Texture coordinates
	uint idTexCoord = 0;
	float* texCoords = nullptr;
	uint idBones = 0;
	std::vector<VertexBoneData> bones;
	std::vector<BoneInfo> boneInfo;

	std::map<std::string, uint> boneNameToIndexMap;

	unsigned int VAO = 0;

	// AABB
	AABB localAABB;

	Shape meshType;
	std::string path;
	bool isAnimated = false;
private:
	// Debug functions for drawing
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;

	void PrimitiveMesh(par_shapes_mesh* primitiveMesh);

private:
	// Debug bools
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;

	const aiScene* assimpScene = nullptr;
};

#endif // !__MESH_H__