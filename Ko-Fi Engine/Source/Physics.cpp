#include "Physics.h"

#include "PxPhysicsAPI.h"

// Module constructor
Physics::Physics(KoFiEngine* engine) : Module()
{
	name = "Physics";
	this->engine = engine;
}

// Module destructor
Physics::~Physics()
{
}

bool Physics::Awake(Json configModule)
{
	return true;
}

bool Physics::Start()
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!foundation)
	{
		//LOG("[ERROR] Physics Module: PxCreateFoundation failed!");
		return false;
	}
	//LOG("Physics Foundation created succesfully");

	return true;
}

bool Physics::PreUpdate(float dt)
{
	return true;
}

bool Physics::Update(float dt)
{
	return true;
}

bool Physics::PostUpdate(float dt)
{
	return true;
}

bool Physics::CleanUp()
{
	return true;
}
