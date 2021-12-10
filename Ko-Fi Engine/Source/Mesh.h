#pragma once
#include <vector>
#include <string>
#include "MathGeoLib/Geometry/AABB.h"

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

typedef unsigned int uint;
class GameObject;

struct Texture
{
	uint textureID = 0;
	int width, height, nrChannels;
	std::string texturePath;
};

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

	// Indices
	uint id_index = 0; // index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	// Vertices
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	// Normals
	uint id_normal = 0;
	uint num_normals = 0;
	float* normals = nullptr;

	// Texture coordinates
	uint id_tex_coord = 0;
	uint num_tex_coords = 0;
	float* tex_coords = nullptr;

	// Texture
	Texture texture;

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