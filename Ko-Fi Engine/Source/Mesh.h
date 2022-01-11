#pragma once
#include <vector>
#include <string>
#include "MathGeoLib/Geometry/AABB.h"

class GameObject;
typedef unsigned int uint;
class Mesh
{
public:
	Mesh();
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
	uint id_index = 0; // index in VRAM
	uint* indices = nullptr;

	// Vertices
	uint id_vertex = 0; // unique vertex in VRAM
	float* vertices = nullptr;

	// Normals
	uint id_normal = 0;
	float* normals = nullptr;

	// Texture coordinates
	uint id_tex_coord = 0;
	float* tex_coords = nullptr;

	unsigned int VAO;

	// Texture

	// AABB
	AABB localAABB;
	
private:
	void SetUpDefaultTexture();
	// Debug functions for drawing
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	// Debug bools
	bool drawVertexNormals = false;
	bool drawFaceNormals = false;
	bool drawColors = false;
	bool drawTextureCoords = false;
};