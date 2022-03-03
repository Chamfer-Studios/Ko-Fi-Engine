#include "Physics.h"

#include "PxPhysicsAPI.h"

#define NBTHREADS 4

physx::PxFilterFlags SampleSubmarineFilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return physx::PxFilterFlag::eDEFAULT;
}

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
	LOG("Initializing Module Physics ------------------------------------------------");

	// PxFoundation object creation
	// Note: PxDefaultErrorCallback: This PhysX class, had no constructor definition, so vs threw some errors on it.
	// To fix it, I just defined empty constructors on that clas... I do not know if it will affect later
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!foundation)
	{
		LOG_BOTH("PxCreateFoundation failed!");
		return false;
	}
	LOG_BOTH("PxCreateFoundation returned successfully!");

	// Top-level PxPhysics object creation
	bool recordMemoryAllocations = true;
	physics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics)
	{
		LOG_BOTH("PxCreatePhysics failed!");
		return false;
	}
	LOG_BOTH("PxCreatePhysics returned successfully!");

	// Cooking creation
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!cooking)
	{
		LOG_BOTH("PxCreateCooking failed!");
		return false;
	}
	LOG_BOTH("PxCreateCooking returned successfully!");

	// Scene description creation
	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -gravity, 0.0f);
	sceneDesc.bounceThresholdVelocity = gravity * 0.2f;
	//if (!sceneDesc.cpuDispatcher)
	//{
	//	PxCpuDispatcher* _cpuDispatcher = PxDefaultCpuDispatcherCreate(nbThreads);
	//	if (!_cpuDispatcher)
	//	{
	//		LOG_BOTH("PxDefaultCpuDispatcherCreate failed!");
	//		return false;
	//	}
	//	LOG_BOTH("PxDefaultCpuDispatcherCreate returned successfully!");
	//	sceneDesc.cpuDispatcher = _cpuDispatcher;
	//}

	sceneDesc.filterShader = SampleSubmarineFilterShader;
	scene = physics->createScene(sceneDesc);
	if (!scene)
	{
		LOG_BOTH("createScene failed!");
		return false;
	}
	LOG_BOTH("createScene returned successfully!");

	LOG("Finished initializing Module Physics ---------------------------------------");
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
	if (foundation)
		foundation->release();
	if (physics)
		physics->release();
	if (cooking)
		cooking->release();
	if (scene)
		scene->release();

	foundation = nullptr;
	physics = nullptr;
	cooking = nullptr;
	scene = nullptr;

	return true;
}
