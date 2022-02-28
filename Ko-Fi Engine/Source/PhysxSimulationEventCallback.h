#ifndef __PHYSX_SIMULATION_EVENT_CALLBACK_H__
#define __PHYSX_SIMULATION_EVENT_CALLBACK_H__

#include "Physics.h"
#include "PxPhysicsAPI.h"

class PhysxSimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
	PhysxSimulationEventCallback();
	~PhysxSimulationEventCallback();

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);

	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) {}

	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}

private:

};

#endif // !__PHYSX_SIMULATION_EVENT_CALLBACK_H__

