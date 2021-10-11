#pragma once
#ifndef FILELOADER_H
#define FILELOADER_H

#include "Module.h"

struct Mesh
{
	uint id_index = 0; // index in VRAM
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;
};

class FileLoader : public Module
{
public:
	FileLoader();
	~FileLoader();

	bool Awake();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
};

#endif FILELOADER_H // FILELOADER_H