#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"

#include "PxPhysicsAPI.h"

namespace physx
{
	class PxBase;
	class PxFoundation;
	class PxPhysics;
	class PxCooking;
	class PxScene;
	class PxMaterial;
	class PxActor;
	class PxActorShape;
	class PxRigidActor;
	class PxRigidStatic;
	class PxSimulationEventCallback;
	class PxQueryFilterCallback;

	typedef uint32_t PxU32;
};

class GameObject;

class Physics : public Module
{
public:
	Physics(KoFiEngine* engine); // Module constructor
	~Physics(); // Module destructor

	// TODO: Serialization -------------------------------
	bool Awake(Json configModule); // Not used, scene gravity is not serialized
	// --------------------------------------------------
	bool Start();
	bool PreUpdate(float dt); // Not used
	bool Update(float dt);
	bool PostUpdate(float dt); // Not used
	bool CleanUp();
	void OnNotify(const Event& event); // Not used

	bool InitializePhysX();

	void AddActor(physx::PxActor* actor, GameObject* owner);
	void DeleteActor(physx::PxActor* actor);
	inline const std::map<physx::PxRigidActor*, GameObject*> GetActors() { return actors; }

	// Getters & setters
	inline physx::PxPhysics* GetPxPhysics() const { return physics; }

	inline bool IsSimulating() { return isSimulating; }

	inline float GetGravity() const { return gravity; }
	inline void SetGravity(const float newGravity) { gravity = newGravity; scene->setGravity(physx::PxVec3(0.0f, -gravity, 0.0f)); }

private:
	KoFiEngine* engine = nullptr;

	bool isSimulating = false;

	std::map<physx::PxRigidActor*, GameObject*> actors;

	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;
	physx::PxScene* scene = nullptr;

	physx::PxU32 nbThreads = 4;

	// Modificable physics attributes
	float gravity = 0.2f;
};

#endif // !__MODULE_PHYSICS_H__

