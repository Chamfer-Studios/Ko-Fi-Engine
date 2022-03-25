#include "Physics.h"

#include "PxPhysicsAPI.h"
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"
#include "SimulationEventCallback.h"

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
	if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING || engine->GetSceneManager()->GetGameState() == GameState::PAUSED)
	{
		isSimulating = true;
	}
	else isSimulating = false;

	if (scene && isSimulating)
	{
		// Maybe we have to refactor physx timing simulation, not sure if this will work as intended
		scene->simulate(engine->GetSceneManager()->GetGameDt());
		scene->fetchResults(true);
	}

	return true;
}

bool Physics::CleanUp()
{
	RELEASE(simulationEventCallback);

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
		int newNbThreads = GetNbThreads();
		if (ImGui::DragInt("##drag_threads", &newNbThreads, 0.1f, 0.0f, 16.0f))
		{
			SetNbThreads(newNbThreads);
			engine->SaveConfiguration();
		}
		ImGui::SameLine();
		ImGui::Text("Number of threads");

		ImGui::Text("");
		float grav = GetGravity();
		if (ImGui::DragFloat("##gravfloatdyn", &grav, 0.1f, -10.0f, 10.0f, "%.2f"))
		{
			SetGravity(grav);
			engine->SaveConfiguration();
		}
		ImGui::SameLine();
		ImGui::Text("Scene gravity");
		if (ImGui::Button("Default gravity##"))
		{
			SetGravity(9.81f);
			engine->SaveConfiguration();
		}

		ImGui::Separator();
		ImGui::Text("Filters:");
		for (int i = 0; i < filters.size(); ++i)
		{
			ImGui::Text(filters[i].c_str());
			ImGui::SameLine();
			std::string label = "delete##";
			label += filters[i];
			if (ImGui::Button(label.c_str()))
			{
				DeleteFilter(filters[i]);
				engine->SaveConfiguration();
			}
		}
		char filterBuff[64];
		strcpy_s(filterBuff, "");
		ImGui::Text("Create filter: ");
		if (ImGui::InputText("##createfilter", filterBuff, IM_ARRAYSIZE(filterBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			std::string str = filterBuff;
			if (str == "")
			{
				LOG_BOTH("ERROR, cannot add an empty filter\n");
			}
			else
			{
				AddFilter(str);
				engine->SaveConfiguration();
			}
		}
		ImGui::Text("(For the input text to work, you will have to write the desired filter name and press enter)");
		ImGui::Separator();
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
		LOG_BOTH("PxCreateFoundation failed!\n");
		return false;
	}

	// Top-level PxPhysics object creation
	bool recordMemoryAllocations = true;
	physics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics)
	{
		LOG_BOTH("PxCreatePhysics failed!\n");
		return false;
	}

	// Cooking creation
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!cooking)
	{
		LOG_BOTH("PxCreateCooking failed!\n");
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
			LOG_BOTH("PxDefaultCpuDispatcherCreate failed!\n");
			return false;
		}
		sceneDesc.cpuDispatcher = _cpuDispatcher;
	}

	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	simulationEventCallback = new SimulationEventCallback(this);
	sceneDesc.simulationEventCallback = simulationEventCallback;
	scene = physics->createScene(sceneDesc);
	if (!scene)
	{
		LOG_BOTH("createScene failed!\n");
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
			LOG_BOTH("ERROR, the filter %s already exists in filters array\n", newFilter.c_str());
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
			{
				filters.erase(i);
				break;
			}	
		}
	}
	else
	{
		LOG_BOTH("ERROR, the filter to delete %s is not contained in filters array\n", deletedFilter.c_str());
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
