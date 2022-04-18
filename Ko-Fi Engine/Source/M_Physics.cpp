#include "M_Physics.h"

#include "PxPhysicsAPI.h"
#include "Globals.h"
#include "Engine.h"
#include "M_SceneManager.h"
#include "SimulationEventCallback.h"

#include <vector>
#include <string>

#include "optick.h"

physx::PxFilterFlags customFilterShader(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	if ((physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))) 
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
	}
	else
	{
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	return physx::PxFilterFlag::eDEFAULT;
}

// Module constructor
M_Physics::M_Physics(KoFiEngine* engine) : Module()
{
	name = "Physics";
	this->engine = engine;
}

// Module destructor
M_Physics::~M_Physics()
{
}

bool M_Physics::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool M_Physics::Start()
{
	bool ret = true;

	LOG_BOTH("Initializing Module M_Physics ------------------------------------------------");

	ret = InitializePhysX();

	LOG_BOTH("Finished initializing Module M_Physics ---------------------------------------");

	return ret;
}

bool M_Physics::Update(float dt)
{
	OPTICK_EVENT();

	if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING || engine->GetSceneManager()->GetGameState() == GameState::PAUSED)
	{
		isSimulating = true;
	}
	else isSimulating = false;

	if (scene && isSimulating)
	{
		// Maybe we have to refactor physx timing simulation, not sure if this will work as intended
		scene->simulate(/*engine->GetSceneManager()->GetGameDt()*/dt);
		scene->fetchResults(true);
	}

	return true;
}

bool M_Physics::CleanUp()
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

	simulationEventCallback = nullptr;

	// TEST: Delete the filter matrix
	DeleteFilterMatrix();

	return true;
}

bool M_Physics::SaveConfiguration(Json& configModule) const
{
	configModule["Gravity"] =  gravity;
	configModule["Number_threads"] = nbThreads;

	configModule["Filters"];
	for (std::string filter : filters)
	{
		configModule["Filters"].push_back(filter);
	}

	// TEST: Save filter matrix
	configModule["Filter_matrix"];
	for (int i = 0; i < filters.size(); ++i)
	{
		for (int j = 0; j < filters.size(); ++j)
		{
			configModule["Filter_matrix"].push_back(filterMatrix[i][j]);
		}
	}

	return true;
}

bool M_Physics::LoadConfiguration(Json& configModule)
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

	//TEST: Delete current filter matrix
	if (filterMatrix)
		DeleteFilterMatrix();

	if (filters.size())
	{
		// TEST: Declare new filter matrix
		DeclareFilterMatrix();

		// TEST: Traverse the 2D array and assign values from json
		int iteratorX = 0;
		int iteratorY = 0;
		for (auto filterMat : configModule.at("Filter_matrix").items())
		{
			filterMatrix[iteratorX][iteratorY] = filterMat.value().get<bool>();

			++iteratorY;
			if (iteratorY >= filters.size())
			{
				++iteratorX;
				iteratorY = 0;
			}
		}
	}

	return true;
}

bool M_Physics::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Physics##"))
	{
		// NUMBER OF THREADS ----------------------------------------------------
		int newNbThreads = GetNbThreads();
		if (ImGui::DragInt("##drag_threads", &newNbThreads, 0.1f, 0.0f, 16.0f))
		{
			SetNbThreads(newNbThreads);
			engine->SaveConfiguration();
		}
		ImGui::SameLine();
		ImGui::Text("Number of threads");
		// ------------------------------------------------------------------------

		// GRAVITY ----------------------------------------------------------------
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
		// ------------------------------------------------------------------------

		// COLLISION FILTERS ------------------------------------------------------
		if (ImGui::TreeNodeEx("Filters"))
		{
			for (int i = 0; i < filters.size(); ++i)
			{
				ImGui::Text(filters[i].c_str());
				if (filters[i] != defaultFilter)
				{
					ImGui::SameLine();
					std::string label = "delete##";
					label += filters[i];
					if (ImGui::Button(label.c_str()))
					{
						DeleteFilter(filters[i]);
						engine->SaveConfiguration();
					}
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

			ImGui::TreePop();
		}
		ImGui::Separator();
		// -------------------------------------------------------------------------------------------------------------

		// FILTER MATRIX ---------------------------------------------------------------------
		if (ImGui::TreeNodeEx("Filter Matrix"))
		{
			size_t filSize = filters.size();
			for (int i = 0; i < filSize; ++i)
			{
				for (int j = 0; j < filSize; ++j)
				{
					if (j != 0) ImGui::SameLine();

					std::string label( "##" + filters[i] + std::to_string(i) + std::to_string(j));
					bool filterMat = filterMatrix[i][j];
					if (ImGui::Checkbox(label.c_str(), &filterMat))
					{
						filterMatrix[i][j] = filterMat;
						engine->SaveConfiguration();
					}
				}
				ImGui::SameLine();
				ImGui::Text(std::string(filters[i] + " (" + std::to_string(i) + ")").c_str());
			}
			int count = filSize - 1;
			for (int i = 0; i < filSize; ++i)
			{
				if (i != 0)
				{
					ImGui::SameLine();
				}
				ImGui::Text(std::string("  " + std::to_string(i) + "      ").c_str());
			}

			ImGui::TreePop();
		}
		ImGui::Separator();
		// -----------------------------------------------------------------------------------
	}

	return true;
}

void M_Physics::OnNotify(const Event& event)
{
}

bool M_Physics::InitializePhysX()
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
	sceneDesc.bounceThresholdVelocity = gravity * 0.02f;
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

	/*sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;*/
	sceneDesc.filterShader = customFilterShader;
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

void M_Physics::AddActor(physx::PxActor* actor, GameObject* owner)
{
	if (actor)
	{
		scene->addActor(*actor);
		actors.insert(std::make_pair<physx::PxRigidActor*, GameObject*>((physx::PxRigidActor*)actor, (GameObject*)(void*)owner));
	}
}

void M_Physics::DeleteActor(physx::PxActor* actor)
{
	if (actor)
	{
		scene->removeActor(*actor);
		actors.erase((physx::PxRigidActor*)actor);
	}
}

void M_Physics::AddFilter(const std::string newFilter)
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

	// TEST: Traverse the 2D array and assign values from old filter matrix and add the new filter
	// First, we want to declare a new filter matrix
	size_t filSize = filters.size();													
	bool** newFilterMatrix = new bool* [filSize];												
	for (int i = 0; i < filSize; ++i) { newFilterMatrix[i] = new bool[filSize]; }
	// Then we want to copy the old filter matrix to the new one, and set to true the incoming filter change
	for (int i = 0; i < filSize; ++i)
	{
		for (int j = 0; j < filSize; ++j)
		{
			// If i or j gets to the end of filterMatrix (new incoming filter), then set to true, else copy from old filter matrix
			if (i == filSize - 1 || j == filSize - 1)
				newFilterMatrix[i][j] = true;
			else
				newFilterMatrix[i][j] = filterMatrix[i][j];
		}
	}
	DeclareFilterMatrix();

	// TEST: Delete current filter matrix
	DeleteFilterMatrix();

	// TEST: Set the new filter matrix
	SetFilterMatrix(newFilterMatrix);
}

void M_Physics::DeleteFilter(const std::string deletedFilter)
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

	// TEST: Traverse the 2D array and assign values from old filter matrix and add the new filter
	// First, we want to declare a new filter matrix
	size_t filSize = filters.size();
	bool** newFilterMatrix = new bool* [filSize];
	for (int i = 0; i < filSize; ++i) { newFilterMatrix[i] = new bool[filSize]; }
	// Then we want to copy the old filter matrix to the new one
	// TODO: We need to re-do the delete logic, as it is not done correctly
	for (int i = 0; i < filSize; ++i)
	{
		if (i != 0) // What I guess this make is that the default filter is kept as true
		{
			for (int j = 0; j < filSize; ++j)
			{
				newFilterMatrix[i][j] = false;
			}
		}
		else
		{
			for (int j = 0; j < filSize; ++j)
				newFilterMatrix[0][j] = true;
		}
			
	}

	// TEST: Delete current filter matrix
	DeleteFilterMatrix();

	// TEST: Set the new filter matrix
	SetFilterMatrix(newFilterMatrix);

}

std::string const M_Physics::GetFilterByID(const uint ID)
{
	if (ID < 0 || ID >= filters.size())
		return defaultFilter;
	else
		return filters[ID];
}

bool M_Physics::Raycast(float3 origin, float3 direction, float maxDistance)
{
	return false;
	physx::PxRaycastBuffer hit;
	bool status = scene->raycast(physx::PxVec3(origin.x, origin.y, origin.z), physx::PxVec3(direction.x, direction.y, direction.z), maxDistance, hit);
	return hit.hasAnyHits();
}

uint const M_Physics::GetFilterID(const std::string* newFilter)
{
	for (int i = 0; i < filters.size(); ++i)
	{
		if (filters[i] == *newFilter)
			return i;
	}

	return -1;
}
