#include "Editor.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Engine.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Primitive.h"
#include "ImGuiAppLog.h"
#include "FileSystem.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentParticle.h"
#include "ComponentScript.h"
#include "Material.h"
#include "ComponentTransform2D.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "node_editor.h"

#include "SDL_assert.h"

// TMP
////////////////////////////////////////////////////////////////////////////////////////////

TMPPlane::TMPPlane(uint uid) : uid(uid), vertexBuf(-1), indexBuf(-1),
normalsBuf(-1)
{
	vertexNum = 0;
	normalNum = 0;
	indexNum = 0;
	CopyParMesh(par_shapes_create_plane(20, 20));
}

TMPPlane::~TMPPlane()
{
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
	normalNum = parMesh->ntriangles;

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

	memcpy(&normals[0], parMesh->normals, vertexNum);

	par_shapes_free_mesh(parMesh);
}

void TMPPlane::GenerateBuffers()
{
	//Vertex
	glGenBuffers(1, (GLuint*)&vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertexNum, &vertices[0], GL_STATIC_DRAW);

	//Normals
	glGenBuffers(1, &normalsBuf);
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertexNum * 3, &normals[0], GL_STATIC_DRAW);

	//Indices
	glGenBuffers(1, &indexBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, &indices[0], GL_STATIC_DRAW);

	if (!texCoords.empty())
	{
		//Textures
		glGenBuffers(1, &textureBuf);
		glBindBuffer(GL_ARRAY_BUFFER, textureBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
	}
}


SceneIntro::SceneIntro(KoFiEngine* engine) : Scene()
{
	name = "SceneIntro";

	// Needed modules
	this->engine = engine;

	jsonHandler.LoadJson(j,"EngineConfig/window_test.json");

	rootGo = new GameObject(-1, engine, "Root");
	rootGo->SetParentUID(rootGo->GetUID());
	gameObjectList.push_back(rootGo);

	LCG random;
	uint uid = random.Int();
	plane = new TMPPlane(uid);
	GameObject * g = this->CreateEmptyGameObject("Particle Test");
	g->CreateComponent<ComponentParticle>();
}

SceneIntro::~SceneIntro()
{
	CleanUp();
}

// Load assets
bool SceneIntro::Start()
{
	bool ret = true;

	CONSOLE_LOG("Loading Intro assets");
	appLog->AddLog("Loading Intro assets\n");

	example::NodeEditorInitialize();

	for (GameObject* go : this->gameObjectList)
	{
		go->Start();
	}

	ComputeQuadTree();
	
	return ret;
}

bool SceneIntro::PreUpdate(float dt)
{
	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() == nullptr)
			go->PreUpdate();
	}

	for (GameObject* go : this->gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() != nullptr)
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
			go->Update(dt);
	}

	//example::NodeEditorShow();
	if (ray.IsFinite())
		//DrawDebugRay(ray);

	if (sceneTree != nullptr && drawSceneTree)
	{
		ComputeQuadTree();
		sceneTree->Draw();
	}
	return true;
}

bool SceneIntro::PostUpdate(float dt)
{
	// Draw meshes
	for (GameObject* go : gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() == nullptr)
			go->PostUpdate(dt); 
	}

	for (GameObject* go : gameObjectList)
	{
		if (go->GetComponent<ComponentTransform2D>() != nullptr)
			go->PostUpdate(dt);
	}

	for (GameObject* parent : gameObjectListToCreate)
	{
		GameObject* bullet = CreateEmptyGameObject("Bullet");
		//parent->GetComponent<ComponentScript>()->handler->lua["bullet"] = bullet;
		//parent->GetComponent<ComponentScript>()->handler->lua.script("table.insert(bullets, bullet)"); //We will need something like this

		bullet->GetTransform()->SetScale(float3(0.1, 0.1, 0.1));
		float3 pos = parent->GetTransform()->GetPosition();
		bullet->GetTransform()->SetPosition(float3(pos.x, pos.y+15, pos.z - 15));
		float3 parentRot = parent->GetTransform()->GetRotation();
		float3 rot = { parentRot.x-55,parentRot.y,parentRot.z };
		bullet->GetTransform()->SetRotation(rot);

		ComponentMesh* componentMesh = bullet->CreateComponent<ComponentMesh>();
 		Mesh* mesh = gameObjectList.at(6)->GetComponent<ComponentMesh>()->GetMesh();
		componentMesh->SetMesh(mesh);

		
		ComponentMaterial* componentMaterial = bullet->CreateComponent<ComponentMaterial>();
		Importer::GetInstance()->textureImporter->Import(nullptr,&componentMaterial->texture);
		Material* material = new Material();
		Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
		componentMaterial->SetMaterial(material);
		
		ComponentScript* componentScript = bullet->CreateComponent<ComponentScript>();
		componentScript->path = "Assets/Scripts/Bullet.lua";
		componentScript->ReloadScript();
		parent->GetComponent<ComponentScript>()->handler->lua["SetBulletDirection"](bullet);
	}
	gameObjectListToCreate.clear();
	for (GameObject* gameObject : gameObjectListToDelete)
	{
		DeleteGameObject(gameObject);
	}
	gameObjectListToDelete.clear();

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