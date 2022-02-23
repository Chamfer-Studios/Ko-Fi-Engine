#pragma once
#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H

#include "Scene.h"
//#include "Globals.h"
#include "JsonHandler.h"

#define MAX_SNAKE 2

class GameObject;
struct PhysBody3D;
struct PhysMotor3D;

class SceneIntro : public Scene
{
public:
	SceneIntro(KoFiEngine* engine);
	~SceneIntro();

	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	int random = 0;
	JsonHandler jsonHandler;
	Json j;

	// Temporal to manage the use of the camera
	GameObject* camera = nullptr;



};

#include "par_shapes.h"

class TMPPlane
{
public:
	TMPPlane(uint uid);
	~TMPPlane();

	void GenerateBuffers();

private:
	void CopyParMesh(par_shapes_mesh* parMesh);

public:
	uint uid = 0;

	uint vertexNum = 0;
	std::vector<float3> vertices;

	uint normalNum = 0;
	std::vector<float3> normals;

	std::vector<float2> texCoords;

	uint indexNum = 0;
	std::vector<uint> indices;

	uint vertexBuf = 0;
	uint indexBuf = 0;
	uint textureBuf = 0;
	uint normalsBuf = 0;

	unsigned int VAO;
};

#endif SCENE_INTRO_H // SCENE_INTRO_H