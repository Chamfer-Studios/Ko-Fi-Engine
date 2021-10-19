#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Module.h"

class Renderer3D;

struct Mesh
{
	uint id_index = 0; // index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;
};

class FileSystem : public Module
{
public:
	FileSystem();
	~FileSystem();

	bool Awake();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void LoadMesh(const char* file_path);
	void GenerateMeshesBuffers();
	void GenerateMeshBuffer(Mesh& mesh);

public:
	std::vector<Mesh> meshes;
};

#endif FILELOADER_H // FILESYSTEM_H