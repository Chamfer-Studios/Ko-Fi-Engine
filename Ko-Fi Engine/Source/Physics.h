#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"
#include "Engine.h"
#include "SceneManager.h"



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

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------
	void OnNotify(const Event& event);
	bool InitializePhysX();

	void AddActor(physx::PxActor* actor, GameObject* owner);
	void DeleteActor(physx::PxActor* actor);
	inline const std::map<physx::PxRigidActor*, GameObject*> GetActors() { return actors; }


	// Getters & setters
	inline physx::PxPhysics* GetPxPhysics() const { return physics; }

	inline bool GetInGame() { return inGame; }

private:
	KoFiEngine* engine = nullptr;

	bool inGame = false;

	std::map<physx::PxRigidActor*, GameObject*> actors;

	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;
	physx::PxScene* scene = nullptr;

	float gravity = 0.2f;

	physx::PxU32 nbThreads = 4;
};

#endif // !__MODULE_PHYSICS_H__

