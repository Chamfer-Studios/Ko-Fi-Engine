#pragma once
#include "Mesh.h"
#include <vector>

class GameObject
{
public:
	GameObject(const char* path, uint id)
	{
		LoadModel(path);
		std::string tmpName = "GameObject";
		tmpName += std::to_string(id);
		name = tmpName;
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