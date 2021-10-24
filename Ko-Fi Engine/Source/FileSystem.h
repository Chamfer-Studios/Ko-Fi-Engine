#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Module.h"
#include <filesystem>
namespace fs = std::filesystem;
class Renderer3D;
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128
struct Mesh
{
	uint VAO;

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

	//FileSystem Functions
	bool OpenFile(const char* path) const;
	bool SaveFile(const char* path) const;
	void EnumerateFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& dirs);
	void AddPath(const char* path);
	void LoadMesh(const char* file_path,std::vector<Mesh>& meshes);
private:
	void GenerateMeshesBuffers(std::vector<Mesh>& meshes);
	void GenerateMeshBuffer(Mesh& mesh);
	void GenerateTextureBuffers(Mesh& mesh);

public:
	fs::path rootPath;

};

#endif FILELOADER_H // FILESYSTEM_H