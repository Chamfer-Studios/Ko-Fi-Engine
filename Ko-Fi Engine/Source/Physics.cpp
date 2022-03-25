#include "Physics.h"

#include "PhysX_4.1/include/PxPhysicsAPI.h"
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"

#include <vector>
#include <string>

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
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool Physics::Start()
{
	bool ret = true;

	LOG_BOTH("Initializing Module Physics ------------------------------------------------");

	ret = InitializePhysX();

	LOG_BOTH("Finished initializing Module Physics ---------------------------------------");

	return ret;
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
		scene->simulate(engine->GetSceneManager()->GetGameDt());
		scene->fetchResults(true);
	}

	return true;
}

bool Physics::CleanUp()
{
	if (foundation)
		foundation->release();
	if (material)
		material->release();
	if (physics)
		physics->release();
	if (cooking)
		cooking->release();
	/*if (scene)
		scene->release();*/

	foundation = nullptr;
	physics = nullptr;
	cooking = nullptr;
	material = nullptr;
	scene = nullptr;

	return true;
}

bool Physics::SaveConfiguration(Json& configModule) const
{
	configModule["Gravity"] =  gravity;
	configModule["Number_threads"] = nbThreads;

	configModule["Filters"];
	//configModule["Filters"].push_back(std::string("hola"));
	/*configModule["Filters"].push_back(std::string("hola soy otro filtro"));*/
	for (std::string filter : filters)
	{
		configModule["Filters"].push_back(filter);
	}

	return true;
}

bool Physics::LoadConfiguration(Json& configModule)
{
	gravity = configModule["Gravity"];
	nbThreads = configModule["Number_threads"];

	// Delete current filters
	if (filters.size() > 0)
		filters.clear();

	// Load new filters
	for (auto filter : configModule.at("Filters").items())
	{
		filters.push_back(filter.value().get<std::string>());
	}

	return true;
}

bool Physics::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Physics##"))
	{
		ImGui::Text("Number of threads");
		ImGui::SameLine();
		int newNbThreads = GetNbThreads();
		if (ImGui::DragInt("##drag_threads", &newNbThreads, 0.1f, 0.0f, 16.0f))
		{
			SetNbThreads(newNbThreads);
			engine->SaveConfiguration();
		}
		ImGui::Separator();
		ImGui::Text("Scene gravity");
		ImGui::SameLine();
		float grav = GetGravity();
		if (ImGui::DragFloat("##gravfloatdyn", &grav, 0.1f, -10.0f, 10.0f, "%.2f"))
		{
			SetGravity(grav);
			engine->SaveConfiguration();
		}
		if (ImGui::Button("Default gravity##"))
		{
			SetGravity(9.81f);
			engine->SaveConfiguration();
		}
	}

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

	// Top-level PxPhysics object creation
	bool recordMemoryAllocations = true;
	physics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics)
	{
		LOG_BOTH("PxCreatePhysics failed!");
		return false;
	}

	// Cooking creation
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!cooking)
	{
		LOG_BOTH("PxCreateCooking failed!");
		return false;
	}

	// Scene description creation
	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -gravity, 0.0f);
	sceneDesc.bounceThresholdVelocity = gravity * 0.25f;
	if (!sceneDesc.cpuDispatcher)
	{
		physx::PxCpuDispatcher* _cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(/*nbThreads*/4);
		if (!_cpuDispatcher)
		{
			LOG_BOTH("PxDefaultCpuDispatcherCreate failed!");
			return false;
		}
		sceneDesc.cpuDispatcher = _cpuDispatcher;
	}

	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
	if (!scene)
	{
		LOG_BOTH("createScene failed!");
		return false;
	}

	material = physics->createMaterial(0, 0, 0);

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

void Physics::AddFilter(const std::string newFilter)
{
	// Filter repeated filters
	for (int i = 0; i < filters.size(); ++i)
	{
		if (filters[i] == newFilter)
		{
			LOG_BOTH("ERROR, the filter %s already exists in filters array", newFilter.c_str());
			return;
		}
	}

	filters.push_back(newFilter);
}

void Physics::DeleteFilter(const std::string deletedFilter)
{
	// Check if it is contained in the filter list
	bool contains = false;
	for (int i = 0; i < filters.size(); ++i)
	{
		if (filters[i] == deletedFilter)
		{
			contains = true;
			break;
		}
	}

	if (contains)
	{
		for (std::vector<std::string>::iterator i = filters.begin(); i != filters.end(); ++i)
		{
			if (*i == deletedFilter)
				filters.erase(i);
		}
	}
	else
	{
		LOG_BOTH("ERROR, the filter to delete %s is not contained in filters array", deletedFilter.c_str());
		return;
	}
}

uint const Physics::GetFilterID(const std::string newFilter)
{
	for (int i = 0; i < filters.size(); ++i)
	{
		if (filters[i] == newFilter)
			return i;
	}

	return -1;
}

std::string const Physics::GetFilterByID(const uint ID)
{
	if (ID < 0 || ID >= filters.size())
		return defaultFilter;
	else
		return filters[ID];
}
