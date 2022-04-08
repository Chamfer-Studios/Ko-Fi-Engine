#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include "PxScene.h"
#include "foundation/PxVec3.h"

class string;
class vector;
class GameObject;
class SimulationEventCallback;
typedef unsigned int uint;

namespace physx
{
	class PxBase;
	class PxFoundation;
	class PxPhysics;
	class PxCooking;
	class PxMaterial;
	class PxScene;
	class PxShape;
	class PxActor;
	class PxRigidActor;
	class PxRigidDynamic;
	class PxRigidStatic;
	class PxForceMode;
	class PxSimulationEventCallback;

	class PxVec3;
	typedef uint32_t PxU32;
};

class Physics : public Module
{
public:
	Physics(KoFiEngine* engine); // Module constructor
	~Physics(); // Module destructor, but module cleaning is done in CleanUp()

	bool Awake(Json configModule);
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	void OnNotify(const Event& event);

	// Physx config initialization, called at Start()
	bool InitializePhysX();

	// Actors methods
	void AddActor(physx::PxActor* actor, GameObject* owner);
	void DeleteActor(physx::PxActor* actor);
	inline std::map<physx::PxRigidActor*, GameObject*> GetActors() { return actors; }

	// Filters methods
	void AddFilter(const std::string newFilter);
	void DeleteFilter(const std::string deletedFilter);
	inline std::vector<std::string> const GetFilters() { return filters; }
	uint const GetFilterID(const std::string* newFilter);
	std::string const GetFilterByID(const uint ID);

	// Filter matrix methods
	inline bool** const GetFilterMatrix() { return filterMatrix; };
	inline void SetFilterMatrix(bool** newFilterMatrix) { filterMatrix = newFilterMatrix; }

	// Getters & setters
	inline physx::PxPhysics* GetPxPhysics() { 
		return physics; 
	}
	inline physx::PxMaterial* GetPxMaterial() const { return material; }

	inline bool IsSimulating() { return isSimulating; }

	inline float GetGravity() const { return gravity; }
	inline void SetGravity(const float newGravity) { gravity = newGravity; scene->setGravity(physx::PxVec3(0.0f, -gravity, 0.0f)); }

	inline int GetNbThreads() const { return nbThreads; }
	inline void SetNbThreads(const float newNbThreads) { nbThreads = newNbThreads; }


	void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 LayerMask, physx::PxU32 filterMask);

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

private:
	KoFiEngine* engine = nullptr;

	bool isSimulating = false;

	std::map<physx::PxRigidActor*, GameObject*> actors;

	std::vector<std::string> filters;
	std::string defaultFilter = "default";
	bool** filterMatrix = nullptr; // For declaring and using a 2d array dynamically, see https://www.geeksforgeeks.org/how-to-declare-a-2d-array-dynamically-in-c-using-new-operator/ as a reference

	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;
	physx::PxMaterial* material = nullptr;
	physx::PxScene* scene = nullptr;
	SimulationEventCallback* simulationEventCallback = nullptr;

	physx::PxU32 nbThreads = 4;

	// Modificable physics attributes
	float gravity = 9.81f;
};

#endif // !__MODULE_PHYSICS_H__

