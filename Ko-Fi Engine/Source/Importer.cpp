#include "Importer.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glew.h"
#include <gl/GL.h>
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Engine.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include <fstream>
#include "I_Material.h"
#include "I_Mesh.h"
#include "I_Scene.h"
#include "I_Shader.h"
#include "I_Texture.h"

Importer* Importer::instance = nullptr;
Importer::Importer()
{
	materialImporter = new I_Material();
	meshImporter = new I_Mesh();
	sceneImporter = new I_Scene();
	shaderImporter = new I_Shader();
	textureImporter = new I_Texture();
}
Importer::~Importer()
{
}

Importer* Importer::GetInstance() {
	
	if (instance == nullptr) {
		instance = new Importer();
	}
		return instance;
	
}

GameObject* Importer::ImportModel(const char* path)
{

}



Mesh* Importer::LoadModel(const char* path)
{

}




