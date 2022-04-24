#ifndef __M_PHYSICS_H__
#define __M_PHYSICS_H__
#include "Module.h"
#include "Globals.h"
#include "reactphysics3d/reactphysics3d.h"
#include <map>
#include <string>

class PhysicsEventListener : public reactphysics3d::EventListener
{
	// Override the onContact() method
	virtual void onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData) override
	{
		//For each contact pair
		for (uint p = 0; p < callbackData.getNbContactPairs(); p++)
		{
			//Get the contact pair
			reactphysics3d::CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
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
	virtual void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) override
	{
		//For each contact pair
		for (uint p = 0; p < callbackData.getNbOverlappingPairs(); p++)
		{
			//Get the Overlapped pair
			reactphysics3d::OverlapCallback::OverlapPair overlapPair = callbackData.getOverlappingPair(p);		

		}
	}
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

	//Engine Config Serialization ----------------------------------
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
public:
	//DynamicsWorld* world = nullptr;

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = nullptr;
	PhysicsEventListener listener;
	std::map<unsigned int, std::string> filters;
	std::string imguiNewFilterText;
	bool** filterMatrix = nullptr;
};

#endif // !__M_PHYSICS_H__
