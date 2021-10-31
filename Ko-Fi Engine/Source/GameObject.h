#pragma once
#include "Mesh.h"
#include <vector>

class GameObject {
public:
	GameObject(const char* path)
	{
		LoadModel(path);
		name = "GameObjectX";
	}
	void Draw();

	void SetName(std::string name);
	std::string GetName();
	std::vector<Mesh>* GetMeshes();

private:
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
	std::string name;
};