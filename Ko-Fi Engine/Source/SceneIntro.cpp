#include "Editor.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Engine.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Primitive.h"
#include <iostream>
#include <fstream>
#include "SDL_assert.h"
#include "RNG.h"
#include "ImGuiAppLog.h"
#include "FileSystem.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentTransform2D.h"
#include "GameObject.h"
//#include "Importer.h"
#include "SceneManager.h"
#include "node_editor.h"

#include "ComponentMaterial.h" // Temporal for the assignment, just to display the texture on the model when the program begins...

TMPPlane::TMPPlane(uint uid) : uid(uid), vertexBuf(-1), indexBuf(-1),
normalsBuf(-1)
{
	vertexNum = 0;
	normalNum = 0;
	indexNum = 0;
	CopyParMesh(par_shapes_create_plane(1, 1));
}

TMPPlane::~TMPPlane()
{
	glDeleteVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBuf);
	vertices.clear();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBuf);
	indices.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normalsBuf);
	normals.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &textureBuf);
	texCoords.clear();
}

void TMPPlane::CopyParMesh(par_shapes_mesh* parMesh)
{
	vertexNum = parMesh->npoints;
	indexNum = parMesh->ntriangles * 3;
	normalNum = parMesh->npoints;

	vertices.resize(vertexNum);
	normals.resize(normalNum);
	indices.resize(indexNum);
	texCoords.resize(vertexNum);

	par_shapes_compute_normals(parMesh);
	for (size_t i = 0; i < vertexNum; ++i)
	{
		memcpy(&vertices[i], &parMesh->points[i * 3], sizeof(float) * 3);
		memcpy(&normals[i], &parMesh->normals[i * 3], sizeof(float) * 3);
		if (parMesh->tcoords != nullptr)
		{
			memcpy(&texCoords[i], &parMesh->tcoords[i * 2], sizeof(float) * 2);
		}
		else
		{
			texCoords.at(i).x = 0.0f;
			texCoords.at(i).y = 0.0f;
		}
	}

	for (size_t i = 0; i < indices.size(); ++i)
	{
		indices[i] = parMesh->triangles[i];
	}

	//memcpy(&normals[0], parMesh->normals, vertexNum);

	par_shapes_free_mesh(parMesh);
}

void TMPPlane::GenerateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Indices
	glGenBuffers(1, &indexBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, &indices[0], GL_STATIC_DRAW);

	//Vertex
	glGenBuffers(1, (GLuint*)&vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertexNum, &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Normals
	glGenBuffers(1, &normalsBuf);
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertexNum * 3, &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	if (!texCoords.empty())
	{
		//Textures
		glGenBuffers(1, &textureBuf);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Unbind any vertex array we have binded before.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

SceneIntro::SceneIntro(KoFiEngine* engine) : Scene()
{
	name = "SceneIntro";
	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");
	rootGo = new GameObject(-1, engine, "Root");
	gameObjectList.push_back(rootGo);
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");
	bool ret = true;
	example::NodeEditorInitialize();

	// Load initial scene (temporal)
	//engine->GetFileSystem()->GameObjectFromMesh("Assets/Models/baker_house.fbx", this->gameObjectList,"Assets/Textures/baker_house.png");

	// REMOVE THE FOLLOWING 2 LINES WHEN WE HAVE THE CUSTOM FILE FORMAT FINISHED.
	//Importer::GetInstance()->ImportModel("Assets/Models/baker_house.fbx");
	/*Importer::GetInstance()->ImportModel("Assets/Models/camera.fbx");
	GameObject* camera = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(2);
	ComponentCamera* componentCamera = camera->CreateComponent<ComponentCamera>();
	camera->AddComponent(componentCamera);*/

	// Load scene with a camera and several houses.
	engine->GetSceneManager()->LoadScene(this, "SceneIntro");

	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() == nullptr)
			go->PreUpdate();
	}

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() == nullptr)
			go->Update();
	}
	//example::NodeEditorShow();

	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() == nullptr)
			go->PostUpdate(dt);
	}

	engine->GetRenderer()->DrawRay();
	return true;
}

// Load assets
bool SceneIntro::CleanUp()
{
	CONSOLE_LOG("Unloading Intro scene");
	appLog->AddLog("Unloading Intro scene\n");

	for (GameObject* gameObject : gameObjectList)
	{
		RELEASE(gameObject);
	}

	example::NodeEditorShutdown();

	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}
