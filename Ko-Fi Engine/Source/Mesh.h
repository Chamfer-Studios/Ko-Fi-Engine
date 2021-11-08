#pragma once

#include <vector>
#include <string>
typedef unsigned int uint;

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128


struct Texture {
	uint textureID = 0;
	int width, height, nrChannels;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	//Mesh Functions
	void SetUpMeshBuffers();
	void SetUpMeshTexture(const char* path);
	void Draw();
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

	//Texture
	Texture texture;
	
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