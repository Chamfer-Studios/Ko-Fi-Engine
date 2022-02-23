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
using Json = nlohmann::json;


class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

	
	void LoadTexture(std::string path = "");
	bool InspectorDraw(PanelChooser* chooser);
	Material* GetMaterial();
	uint GetShader();
	const char* GetShaderPath() { return shaderPath.c_str(); }
	void SetShaderPath(std::string path) { this->shaderPath = path; }
	void LoadShader(const char* shaderPath = nullptr);
	//Material Handling
	void LoadMaterial(const char* path="");
	Json Save() override;
	void Compile();

private:
	void LoadDefaultMaterial();
	void LoadUniforms();
private:
	
	Material* material;
	uint materialShader = 0;
	std::string shaderPath = "Assets/Shaders/default_shader.glsl";
	int currentTextureId=0;
};