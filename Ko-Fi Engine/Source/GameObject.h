#pragma once
#include "Mesh.h"
#include <vector>
class GameObject {
public:
	GameObject(const char* path)
	{
		LoadModel(path);
	}
	void Draw();
private:
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
};