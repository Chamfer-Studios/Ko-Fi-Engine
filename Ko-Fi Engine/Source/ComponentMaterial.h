#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <string>
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

typedef unsigned int uint;

class PanelChooser;

struct Uniform {
	uint type;
	std::string name;
};
class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	
	void LoadTexture(std::string path = "");
	bool InspectorDraw(PanelChooser* chooser);
	Texture GetTexture();
	Material GetMaterial();
	uint GetShader();
	void LoadShader(const char* shaderPath = nullptr);
	//Material Handling
	void LoadMaterial(const char* path="");
	void Compile();

private:
	void LoadDefaultMaterial();
	void LoadUniforms();
private:
	std::vector<Uniform> uniforms;
	Material material;
	uint materialShader = 0;
	std::string shaderPath = "Assets/Shaders/default_shader.glsl";
};