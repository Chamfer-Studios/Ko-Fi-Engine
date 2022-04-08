#ifndef __SIMULATION_CALLBACK_H__
#define __SIMULATION_CALLBACK_H__

#include "PxPhysicsAPI.h"

class M_Physics;

class SimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
	SimulationEventCallback(M_Physics* callback) { this->callback = callback; }
	~SimulationEventCallback() {}

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);

	// Needed for the abstract class PxSimulationEventCallback, but not used
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) {}

	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}

private:
	M_Physics* callback = nullptr;

};

#endif // !__SIMULATION_CALLBACK_H__

