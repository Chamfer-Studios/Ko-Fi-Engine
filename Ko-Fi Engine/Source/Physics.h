#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"



namespace physx
{
	class PxBase;
	class PxFoundation;
	class PxPhysics;
	class PxCooking;
	class PxScene;
	class PxMaterial;
	class PxActor;
	class PxActorShape;
	class PxRigidActor;
	class PxRigidStatic;
	class PxSimulationEventCallback;
	class PxQueryFilterCallback;

	typedef uint32_t PxU32;
};

class GameObject;

class Physics : public Module
{
public:
	Physics(KoFiEngine* engine); // Module constructor
	~Physics(); // Module destructor

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	//void AddActor(physx::PxActor* actor);
	//void DeleteActor(physx::PxActor* actor);


private:
	KoFiEngine* engine = nullptr;

	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;
	physx::PxScene* scene = nullptr;

	float gravity = 9.8f;

	physx::PxU32 nbThreads = 4;
};

#endif // !__MODULE_PHYSICS_H__

