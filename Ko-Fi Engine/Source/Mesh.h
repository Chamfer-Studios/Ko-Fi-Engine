#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <map>
#include <string>

#include "MathGeoLib/Geometry/AABB.h"

//#include "Resource.h"
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
	uint BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
	float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
	VertexBoneData() {}
	void AddBoneData(uint boneID, float weight)
	{
		for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); ++i) 
		{
			if (Weights[i] == 0.0)
			{
				BoneIDs[i] = boneID;
				Weights[i] = weight;
				CONSOLE_LOG("bone %d weight %f index %i\n",boneID,weight,i);
				return;
			}
		}
		assert(0);
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
	
	std::vector<VertexBoneData> bones;

	std::map<std::string, uint> boneNameToIndexMap;

	unsigned int VAO = 0;

	// AABB
	AABB localAABB;

	Shape meshType;
	std::string path;

private:
	// Debug functions for drawing
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;

	void PrimitiveMesh(par_shapes_mesh* primitiveMesh);

private:
	// Debug bools
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
};

#endif // !__MESH_H__