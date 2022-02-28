#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxMaterial;
	class PxRigidActor;
	class PxSimulationEventCallback;
	class PxActor;
	class PxControllerManager;
	class PxCooking;
};

class GameObject;

class Physics : public Module
{
public:
	Physics(KoFiEngine* engine); // Module constructor
	~Physics(); // Module destructor

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnGui() override;
	// void OnSave(JSONWriter& writer) const override;
	// void OnLoad(const JSONReader& reader) override;

private:
	KoFiEngine* engine = nullptr;

	physx::PxFoundation* foundation = nullptr;
};

#endif // !__MODULE_PHYSICS_H__

