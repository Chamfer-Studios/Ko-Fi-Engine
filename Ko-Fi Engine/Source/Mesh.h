#pragma once
#include <vector>
#include <string>
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
typedef unsigned int uint;

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256
class Mesh {
public:
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
	uint textureId = 0;
	uint id_tex_coord = 0;
	uint num_tex_coords = 0;
	float* tex_coords = nullptr;

	uint VAO;
	Mesh();
	~Mesh();
	void SetUpMesh();
	void Draw();
private:
	unsigned int VBO, EBO;

	void SetUpTexture();
};