#include "M_Physics.h"
#include "Engine.h"
#include "GameObject.h"
#include "C_Script.h"
#include "Scripting.h"
#include "C_Info.h"
#include "M_SceneManager.h"
#include "glew.h"
#include <imgui_stdlib.h>


M_Physics::M_Physics(KoFiEngine* engine)
{
	this->name = "Physics";
	this->engine = engine;
	//World Settings
	reactphysics3d::PhysicsWorld::WorldSettings worldSettings;
	worldSettings.gravity = reactphysics3d::Vector3(0, -9.81f, 0);

	// Create the physics world 
	world = physicsCommon.createPhysicsWorld(worldSettings);
	world->setIsDebugRenderingEnabled(true);
	reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();

	// Select the contact points and contact normals to be displayed 
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
	//Set Event Listener
	world->setEventListener(&listener);
}

M_Physics::~M_Physics()
{
}

bool M_Physics::Awake(Json configModule)
{
	LoadConfiguration(configModule);

	return true;
}

bool M_Physics::Start()
{

	return true;
}

bool M_Physics::Update(float dt)
{
	if (engine->GetSceneManager()->GetGameState() == GameState::PLAYING)
	{
		world->update(dt);
	}
	else {
		world->update(0.000000001f);
	}

	return true;
}

bool M_Physics::RenderPhysics()
{
	if (debugPhysics)
	{
		reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
		int numLines = debugRenderer.getNbLines();
		int numTriangles = debugRenderer.getNbTriangles();

		const reactphysics3d::Array<reactphysics3d::DebugRenderer::DebugLine>& lines = debugRenderer.getLines();
		const reactphysics3d::Array<reactphysics3d::DebugRenderer::DebugTriangle>& triangles = debugRenderer.getTriangles();

		for (int i = 0; i < numLines; ++i)
		{
			glBegin(GL_LINES);
			glVertex2f(lines[i].point1.x, lines[i].point1.y);
			glVertex2f(lines[i].point2.x, lines[i].point2.y);
			glEnd();
		}
		for (int i = 0; i < numTriangles; ++i)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBegin(GL_TRIANGLES);
			glColor3f(0.49f, 1.0f, 212.0f);
			glVertex3f(triangles[i].point1.x, triangles[i].point1.y, triangles[i].point1.z);
			glVertex3f(triangles[i].point2.x, triangles[i].point2.y, triangles[i].point2.z);
			glVertex3f(triangles[i].point3.x, triangles[i].point3.y, triangles[i].point3.z);
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}
	}

	return true;
}

bool M_Physics::CleanUp()
{
	// Destroy a physics world 
	if (world)
		physicsCommon.destroyPhysicsWorld(world);

	if (filterMatrix)
		DeleteFilterMatrix();

	return true;
}

bool M_Physics::SaveConfiguration(Json& configModule) const
{
	configModule["Gravity"] = { world->getGravity().x, world->getGravity().y,world->getGravity().z };
	configModule["Filters"] = filters;
	configModule["debugPhysics"] = debugPhysics;
	//Save filter matrix
	configModule["Filter_Matrix"];
	for (int i = 0; i < filters.size(); ++i)
	{
		for (int j = 0; j < filters.size(); ++j)
		{
			configModule["Filter_Matrix"].push_back(filterMatrix[i][j]);
		}
	}
	return true;
}

bool M_Physics::LoadConfiguration(Json& configModule)
{
	if (configModule.contains("Gravity"))
	{
		world->setGravity(reactphysics3d::Vector3(configModule.at("Gravity")[0], configModule.at("Gravity")[1], configModule.at("Gravity")[2]));
	}
	if (configModule.contains("Filters"))
	{
		filters = configModule.at("Filters").get<std::map<unsigned int, std::string>>();
	}
	if (configModule.contains("debugPhysics"))
	{
		debugPhysics = configModule.at("debugPhysics");
	}
	if (configModule.contains("Filter_Matrix"))
	{
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
			for (auto filterMat : configModule.at("Filter_Matrix").items())
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
		for (auto go : engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
		{
			if (go->GetComponent<C_BoxCollider>())
				go->GetComponent<C_BoxCollider>()->UpdateFilter();
		}
	}

	return true;
}

bool M_Physics::InspectorDraw()
{
	if (ImGui::CollapsingHeader("Physics##"))
	{
		if (ImGui::TreeNodeEx("Filters"))
		{
			// Filter list
			auto iter = filters.begin();
			while (iter != filters.end())
			{
				ImGui::Text(iter->second.c_str());
				ImGui::SameLine();
				std::string label = "delete##";
				label += iter->first;
				if (ImGui::Button(label.c_str()))
				{
					RemoveFilter(iter->second);
					engine->SaveConfiguration();
					break;
				}

				++iter;
			}

			ImGui::Text("Create Filter: ");
			ImGui::InputText("##addFilter", &imguiNewFilterText);
			ImGui::SameLine();
			if (ImGui::Button("Add Filter"))
			{
				AddFilter(imguiNewFilterText);
				engine->SaveConfiguration();
			}

			ImGui::Separator();

			// Filter matrix
			size_t filSize = filters.size();
			for (auto iterI = filters.begin(); iterI != filters.end(); ++iterI)
			{
				int i = std::distance(filters.begin(), iterI);
				for (auto iterJ = filters.begin(); iterJ != filters.end(); ++iterJ)
				{
					int j = std::distance(filters.begin(), iterJ);
					if (j != 0) ImGui::SameLine();
					std::string label("##" + iterI->second + std::to_string(i) + std::to_string(j));
					bool filterMat = filterMatrix[i][j];
					if (ImGui::Checkbox(label.c_str(), &filterMat))
					{
						filterMatrix[i][j] = filterMat;
						engine->SaveConfiguration();
						for (auto go : engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
						{
							if (go->GetComponent<C_BoxCollider>())
								go->GetComponent<C_BoxCollider>()->UpdateFilter();
						}
					}

				}
				ImGui::SameLine();
				ImGui::Text(std::string(iterI->second + " (" + std::to_string(i) + ")").c_str());

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

		// Debug physics (draw colliders)
		bool newDebugPhysics = IsDebugPhysics();
		if (ImGui::Checkbox("Debug Physics##", &newDebugPhysics))
			DebugPhysics(newDebugPhysics);
	}

	return true;
}

void M_Physics::OnNotify(const Event& event)
{
}

void M_Physics::AddFilter(std::string newFilter)
{
	filters.emplace(filters.size() + 1, newFilter);
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

void M_Physics::RemoveFilter(std::string filterToRemove)
{
	for (auto it = filters.begin(); it != filters.end(); ++it)
	{
		if (it->second == filterToRemove)
		{
			filters.erase(it);
			break;
		}
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

unsigned int M_Physics::GetFilter(std::string filter)
{
	for (auto it = filters.begin(); it != filters.end(); ++it)
	{
		if (it->second == filter)
		{
			return it->first;
		}
	}
}

reactphysics3d::RigidBody* M_Physics::AddBody(reactphysics3d::Transform rbTransform, GameObject* owner)
{

	reactphysics3d::RigidBody* body = world->createRigidBody(rbTransform);
	collisionBodyToObjectMap.emplace(body, owner);

	return body;
}

void M_Physics::DeleteBodyFromObjectMap(GameObject* go)
{
	for (std::map<reactphysics3d::CollisionBody*, GameObject*>::iterator mapIt = collisionBodyToObjectMap.begin(); mapIt != collisionBodyToObjectMap.end(); ++mapIt)
	{
		if ((*mapIt).second == go)
		{
			collisionBodyToObjectMap.erase(mapIt);
			break;
		}
	}
}

void M_Physics::RayCastHits(float3 startPoint, float3 endPoint, std::string filterName, GameObject* senderGo, std::string uid, sol::function* callback)
{
	// Create the ray 
	reactphysics3d::Vector3 sPoint(startPoint.x, startPoint.y, startPoint.z);
	reactphysics3d::Vector3 ePoint(endPoint.x, endPoint.y, endPoint.z);
	reactphysics3d::Ray ray(sPoint, ePoint);

	// Create an instance of your callback class 
	CustomRayCastCallback callbackObject = CustomRayCastCallback(senderGo, uid, callback);
	unsigned int mask = 0;
	for (auto filter : filters)
	{
		if (filter.second == filterName)

		{
			mask += filter.first;
		}
	}

	// Raycast test 
	world->raycast(ray, &callbackObject, mask);
}

bool M_Physics::CustomRayCastQuery(float3 startPoint, float3 endPoint, TAG tag)
{
	std::vector<GameObject*> gameObjects = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;

	std::vector<GameObject*> candidates;

	for (std::vector<GameObject*>::iterator go = gameObjects.begin(); go != gameObjects.end(); go++)
	{
		GameObject* gameObject = (*go);
		C_Mesh* cMesh = gameObject->GetComponent<C_Mesh>();
		if (!cMesh)
			continue;
		float3 middlePoint = startPoint + (endPoint - startPoint) / 2;
		float3 closest = cMesh->GetGlobalAABB().ClosestPoint(middlePoint);
		float distance = middlePoint.DistanceSq(closest);
		float sCullingRadius = startPoint.Distance(endPoint) / 2;
		if (distance < (sCullingRadius * sCullingRadius))
		{
			candidates.push_back(gameObject);
		}
	}
	
	float3 dir = (endPoint - startPoint).Normalized();
	Ray ray = Ray(startPoint, dir);

	for (std::vector<GameObject*>::iterator go = candidates.begin(); go != candidates.end(); go++) {
		OBB obb = (*go)->GetComponent<C_Mesh>()->obb;
		
		if ((*go)->tag == tag)
		{
			float dNear, dFar;
			if (obb.Intersects(ray, dNear, dFar))
			{
				float3 pNear = startPoint + dir * dNear;
				float3 pFar = startPoint + dir * dFar;
				float endDistance = startPoint.DistanceSq(endPoint);
				float pNearDistance = startPoint.DistanceSq(pNear);
				float pFarDistance = startPoint.DistanceSq(pFar);

				if (pNearDistance < endDistance && pFarDistance < endDistance)
					return true;
			}
		}
	}

	return false;
}

PhysicsEventListener::PhysicsEventListener(M_Physics* mPhysics)
{
	this->mPhysics = mPhysics;
}

void PhysicsEventListener::onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData)
{
	//For each contact pair
	for (uint p = 0; p < callbackData.getNbContactPairs(); p++)
	{
		//Get the contact pair
		reactphysics3d::CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
		GameObject* go1 = nullptr;
		GameObject* go2 = nullptr;

		go1 = mPhysics->GetGameObjectFromBody(contactPair.getBody1());
		go2 = mPhysics->GetGameObjectFromBody(contactPair.getBody2());

		if (go1->GetEngine()->GetSceneManager()->GetGameState() != GameState::PLAYING)
			return;

		if (go1 && go2)
		{

			if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionEnter = sol::protected_function(script->s->handler->lua["OnCollisionEnter"]);
						if (onCollisionEnter.valid()) {
							sol::protected_function_result result = onCollisionEnter(go2);
							if (result.valid()) {
								// Call succeeded
							}
							else {
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionEnter = sol::protected_function(script->s->handler->lua["OnCollisionEnter"]);
						if (onCollisionEnter.valid()) {
							sol::protected_function_result result = onCollisionEnter(go1);
							if (result.valid()) 
							{
								// Call succeeded
							}
							else 
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}
			else if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionRepeat = sol::protected_function(script->s->handler->lua["OnCollisionRepeat"]);
						if (onCollisionRepeat.valid()) {
							sol::protected_function_result result = onCollisionRepeat(go2);
							if (result.valid()) 
							{
								// Call succeeded
							}
							else 
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionRepeat = sol::protected_function(script->s->handler->lua["OnCollisionRepeat"]);
						if (onCollisionRepeat.valid()) {
							sol::protected_function_result result = onCollisionRepeat(go1);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}
			else if (contactPair.getEventType() == reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionExit = sol::protected_function(script->s->handler->lua["OnCollisionExit"]);
						if (onCollisionExit.valid()) {
							sol::protected_function_result result = onCollisionExit(go2);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onCollisionExit = sol::protected_function(script->s->handler->lua["OnCollisionExit"]);
						if (onCollisionExit.valid()) {
							sol::protected_function_result result = onCollisionExit(go1);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}


		}
		//For each contact point of the contact pair
		for (uint c = 0; c < contactPair.getNbContactPoints(); c++)
		{
			//Get the contact point 
			reactphysics3d::CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);
			//Get the contact point on the first collider and convert it in world-space
			reactphysics3d::Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
		}

	}
}

void PhysicsEventListener::onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData)
{
	//For each contact pair
	for (uint p = 0; p < callbackData.getNbOverlappingPairs(); p++)
	{
		//Get the overlapped pair
		reactphysics3d::OverlapCallback::OverlapPair overlapPair = callbackData.getOverlappingPair(p);
		GameObject* go1 = nullptr;
		GameObject* go2 = nullptr;

		go1 = mPhysics->GetGameObjectFromBody(overlapPair.getBody1());
		go2 = mPhysics->GetGameObjectFromBody(overlapPair.getBody2());

		if (go1->GetEngine()->GetSceneManager()->GetGameState() != GameState::PLAYING)
			return;

		if (go1 && go2)
		{

			if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerEnter = sol::protected_function(script->s->handler->lua["OnTriggerEnter"]);
						if (onTriggerEnter.valid()) {
							sol::protected_function_result result = onTriggerEnter(go2);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerEnter = sol::protected_function(script->s->handler->lua["OnTriggerEnter"]);
						if (onTriggerEnter.valid()) {
							sol::protected_function_result result = onTriggerEnter(go1);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}
			else if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerStay = sol::protected_function(script->s->handler->lua["OnTriggerStay"]);
						if (onTriggerStay.valid()) {
							sol::protected_function_result result = onTriggerStay(go2);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerStay = sol::protected_function(script->s->handler->lua["OnTriggerStay"]);
						if (onTriggerStay.valid()) {
							sol::protected_function_result result = onTriggerStay(go1);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}
			else if (overlapPair.getEventType() == reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit)
			{
				for (Component* component : go1->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerExit = sol::protected_function(script->s->handler->lua["OnTriggerExit"]);
						if (onTriggerExit.valid()) {
							sol::protected_function_result result = onTriggerExit(go2);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
				for (Component* component : go2->GetComponents()) // This method used because there could be multiple scripts in one go
				{
					if (component->GetType() != ComponentType::SCRIPT)
						continue;
					C_Script* script = (C_Script*)component;
					if (!script->s->path.empty())
					{
						sol::protected_function onTriggerExit = sol::protected_function(script->s->handler->lua["OnTriggerExit"]);
						if (onTriggerExit.valid()) {
							sol::protected_function_result result = onTriggerExit(go1);
							if (result.valid())
							{
								// Call succeeded
							}
							else
							{
								// Call failed
								sol::error err = result;
								std::string what = err.what();
								appLog->AddLog("%s\n", what.c_str());
							}
						}
					}
				}
			}
		}
	}

}

CustomRayCastCallback::CustomRayCastCallback(GameObject* raycastSender, std::string _uid, sol::function* _callback)
{
	this->raycastSender = raycastSender;
	this->callback = _callback;
	this->uid = _uid;
}

reactphysics3d::decimal CustomRayCastCallback::notifyRaycastHit(const reactphysics3d::RaycastInfo& info)
{


	for (Component* component : raycastSender->GetComponents()) // This method used because there could be multiple scripts in one go
	{
		if (component->GetType() != ComponentType::SCRIPT)
			continue;
		C_Script* script = (C_Script*)component;
		if (!script->s->path.empty()) {
			auto onRayCastHit = sol::protected_function(script->s->handler->lua["OnRayCastHit"]);
			if (onRayCastHit.valid())
				script->s->handler->lua["OnRayCastHit"]();
		}
	}

	if (this->callback != nullptr) this->callback->call(uid);

	// Return a fraction of 1.0 to gather all hits 
	return reactphysics3d::decimal(1.0);
}