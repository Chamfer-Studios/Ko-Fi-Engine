#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <string>
#include "MathGeoLib/Geometry/AABB.h"
#include "Resource.h"
#include "par_shapes.h"


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

class GameObject;

class Mesh : public Resource
{
public:
	Mesh(Shape shape = Shape::NONE);
	~Mesh();

	// Mesh Functions
	void SetUpMeshBuffers();
	void Draw(GameObject* owner);
	void DebugDraw();

	void ToggleVertexNormals();
	void ToggleFacesNormals();

	//SizeBytes
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

	// Texture

	// AABB
	AABB localAABB;

	Shape meshType;

private:
	void SetUpDefaultTexture();
	// Debug functions for drawing
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	void PrimitiveMesh(par_shapes_mesh* primitiveMesh);
	// Debug bools
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
	bool drawColors = false;
	bool drawTextureCoords = false;
};

#endif // !__MESH_H__