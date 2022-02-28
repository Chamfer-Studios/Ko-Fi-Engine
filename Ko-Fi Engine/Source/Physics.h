#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"

#include "PxPhysicsAPI.h"
#include "PhysxSimulationEventCallback.h"

namespace physx
{
	class PxBase;
	class PxPhysics;
	class PxFoundation;
	class PxCooking;
	class PxScene;
	class PxMaterial;
	class PxActor;
	class PxActorShape;
	class PxRigidActor;
	class PxRigidStatic;
	class PxSimulationEventCallback;
	class PxQueryFilterCallback;
	class PxDefaultErrorCallback;

	typedef uint32_t PxU32;
};

class GameObject;
class PhysxSimulationEventCallback;

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




private:
	KoFiEngine* engine = nullptr;

	// PhysX attributes
	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;
	physx::PxScene* scene = nullptr;

	PhysxSimulationEventCallback* simulationCallback = nullptr;
};

#endif // !__MODULE_PHYSICS_H__

