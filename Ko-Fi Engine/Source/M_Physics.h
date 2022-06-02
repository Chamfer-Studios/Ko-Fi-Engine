#ifndef __M_PHYSICS_H__
#define __M_PHYSICS_H__
#include "Module.h"
#include "Globals.h"
#include "reactphysics3d/reactphysics3d.h"
#include <map>
#include <string>
#include "MathGeoLib/Math/float3.h"
#include "sol.hpp"

class GameObject;
class M_Physics;
enum class TAG;
class PhysicsEventListener : public reactphysics3d::EventListener
{
public:
	PhysicsEventListener(M_Physics* mPhysics);
	// Override the onContact() method
	virtual void onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData) override;
	virtual void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) override;
private:
	M_Physics* mPhysics = nullptr;
};

// Class WorldRaycastCallback 
class CustomRayCastCallback : public reactphysics3d::RaycastCallback {

public:
	CustomRayCastCallback(GameObject* raycastSender, std::string _uid = "", sol::function* _callback = nullptr);
	virtual reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& info);
public:
	GameObject* raycastSender = nullptr;
	sol::function* callback = nullptr;
	std::string uid = "";
};

class M_Physics : public Module
{
public:
	//Constructor
	M_Physics(KoFiEngine* engine);

	//Destructor
	~M_Physics();

	//Called before render is avilable
	bool Awake(Json configModule);
	bool Start();

	bool Update(float dt);
	bool RenderPhysics();
	//Called before quitting
	bool CleanUp();

	//Engine Config Serialization -----------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	//---------------------------------------------------------------

	//Engine Config Inspector draw ----------------------------------
	bool InspectorDraw() override;
	// --------------------------------------------------------------

	// Method to receive and manage events
	void OnNotify(const Event& event);

	//Getters and Setters
	inline reactphysics3d::PhysicsWorld* GetWorld() { return this->world; }
	inline reactphysics3d::PhysicsCommon& GetPhysicsCommon() { return this->physicsCommon; }

	//Filters
	void AddFilter(std::string newFilter);
	void RemoveFilter(std::string filterToRemove);
	unsigned int GetFilter(std::string filter);
	inline std::map<unsigned int, std::string> GetFiltersMap() { return filters; }
	inline bool** GetFilterMatrix() { return filterMatrix; }
	//Utils
	reactphysics3d::RigidBody* AddBody(reactphysics3d::Transform rbTransform, GameObject* owner);
	GameObject* GetGameObjectFromBody(reactphysics3d::CollisionBody* collisionBody) { return collisionBodyToObjectMap[collisionBody]; }

	inline void ResetCollisionBodyToObjectMap() { collisionBodyToObjectMap.clear(); }
	void DeleteBodyFromObjectMap(GameObject* go);

	//RayCast
	void RayCastHits(float3 startPoint, float3 endPoint, std::string filterName, GameObject* senderGo, std::string uid = "", sol::function * callback = nullptr);
	bool CustomRayCastQuery(float3 startPoint, float3 endPoint, TAG tag);

	inline bool IsDebugPhysics() const { return debugPhysics; };
	inline void DebugPhysics(const bool newDebugPhysics) { debugPhysics = newDebugPhysics; }
private:
	// Filter matrix private methods
	inline void DeleteFilterMatrix()
	{
		for (int i = 0; i < filters.size(); i++)
			delete[] filterMatrix[i];				// To delete the inner arrays
		delete[] filterMatrix;						// To delete the outer array, which contained the pointers of all the inner arrays
	}
	inline void DeclareFilterMatrix()
	{
		size_t filSize = filters.size();													// Dimensions of the array
		filterMatrix = new bool* [filSize];												// Declare a memory block of bools of filters size
		for (int i = 0; i < filSize; ++i) { filterMatrix[i] = new bool[filSize]; }		// Declare a memory block of bools of filters size
	};
	inline void SetFilterMatrix(bool** newFilterMatrix) { filterMatrix = newFilterMatrix; }

private:
	KoFiEngine* engine = nullptr;
	//DynamicsWorld* world = nullptr;

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = nullptr;
	PhysicsEventListener listener = PhysicsEventListener(this);
	std::map<unsigned int, std::string> filters;
	std::map<reactphysics3d::CollisionBody*, GameObject*> collisionBodyToObjectMap;
	std::string imguiNewFilterText;
	bool** filterMatrix = nullptr;
	bool debugPhysics = true;
};

#endif // !__M_PHYSICS_H__
