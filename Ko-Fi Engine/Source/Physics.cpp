#include "Physics.h"

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

// Serialization: Not used, scene gravity is not serialized
bool Physics::Awake(Json configModule)
{
	return true;
}

bool Physics::Start()
{
	bool ret = true;

	LOG_BOTH("Initializing Module Physics ------------------------------------------------");

	ret = InitializePhysX();

	LOG_BOTH("Finished initializing Module Physics ---------------------------------------");

	return ret;
}

// Not used
bool Physics::PreUpdate(float dt)
{
	return true;
}

bool Physics::Update(float dt)
{
	if (engine->GetSceneManager()->GetState() == RuntimeState::PLAYING || engine->GetSceneManager()->GetState() == RuntimeState::PAUSED)
	{
		isSimulating = true;
	}
	else isSimulating = false;

	if (scene && isSimulating)
	{
		// TODO: WE HAVE TO PASS AS A PARAMETER THE GAME DT, NOT THE ENGINE DT
		scene->simulate(dt);
		scene->fetchResults(true);
	}

	return true;
}

// Not used
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
	// This pointer release crashes at cleaning, I suppose it is due physx itself does its own cleanup or something
	/*if (scene)
		scene->release();*/

	foundation = nullptr;
	physics = nullptr;
	cooking = nullptr;
	scene = nullptr;

	return true;
}

bool Physics::SaveConfiguration(Json& configModule) const
{
	configModule["Gravity"] =  gravity;
	return true;
}

bool Physics::LoadConfiguration(Json& configModule)
{
	gravity = configModule["Gravity"];
	return true;
}

void Physics::OnNotify(const Event& event)
{
}

bool Physics::InitializePhysX()
{
	// PxFoundation object creation
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
	sceneDesc.bounceThresholdVelocity = gravity * 0.25f;
	if (!sceneDesc.cpuDispatcher)
	{
		physx::PxCpuDispatcher* _cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(nbThreads);
		if (!_cpuDispatcher)
		{
			LOG_BOTH("PxDefaultCpuDispatcherCreate failed!");
			return false;
		}
		LOG_BOTH("PxDefaultCpuDispatcherCreate returned successfully!");
		sceneDesc.cpuDispatcher = _cpuDispatcher;
	}

	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
	if (!scene)
	{
		LOG_BOTH("createScene failed!");
		return false;
	}
	LOG_BOTH("createScene returned successfully!");

	return true;
}

void Physics::AddActor(physx::PxActor* actor, GameObject* owner)
{
	if (actor)
	{
		scene->addActor(*actor);
		actors.insert(std::make_pair<physx::PxRigidActor*, GameObject*>((physx::PxRigidActor*)actor, (GameObject*)(void*)owner));
	}
}

void Physics::DeleteActor(physx::PxActor* actor)
{
	if (actor)
	{
		scene->removeActor(*actor);
		actors.erase((physx::PxRigidActor*)actor);
	}
}
