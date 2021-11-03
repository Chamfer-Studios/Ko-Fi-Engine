#pragma once
#include "Mesh.h"
#include <vector>

class GameObject
{
public:
	GameObject(const char* path, uint id);
	void Draw();

	void SetName(std::string name);
	std::string GetName();
	std::vector<Mesh>* GetMeshes();
	void SetNewTexture(const char* path);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
	void LoadMesh(std::string path);
	std::string name;
	uint id;
};