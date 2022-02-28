#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <string>
#include "MathGeoLib/Geometry/AABB.h"
//#include "Resource.h"
#include "par_shapes.h"
#include "Globals.h"

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

class Mesh
{
public:
	Mesh(Shape shape = Shape::NONE);
	~Mesh();

	// Mesh Functions
	void SetUpMeshBuffers();
	void Draw(GameObject* owner);
	void DebugDraw();

	void SetVertexNormals(bool vertex);
	void SetFaceNormals(bool faces);

	bool GetVertexNormals();
	bool GetFaceNormals();

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

	unsigned int VAO = 0;

	// AABB
	AABB localAABB;

	Shape meshType;
	std::string path = "";

private:
	void PrimitiveMesh(par_shapes_mesh* primitiveMesh);

	// Debug functions for drawing
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;

	// Debug bools
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
};

#endif // !__MESH_H__