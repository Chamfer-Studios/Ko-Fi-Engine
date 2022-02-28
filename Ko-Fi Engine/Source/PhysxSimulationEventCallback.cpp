#include "PhysxSimulationEventCallback.h"
#include "Engine.h"
#include "GameObject.h"

PhysxSimulationEventCallback::PhysxSimulationEventCallback()
{
}

PhysxSimulationEventCallback::~PhysxSimulationEventCallback()
{
}

void PhysxSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	LOG_BOTH("THERE HAS BEEN A CONTACT!:");
	for (physx::PxU32 i = 0; i < nbPairs; ++i)
	{
		const physx::PxContactPair& cPair = pairs[i];

		GameObject* gameObject1 = nullptr;
		GameObject* gameObject2 = nullptr;
		//gameObject1 = App->physics->actors[(physx::PxRigidDynamic*)pairHeader.actors[0]];
		//gameObject2 = App->physics->actors[(physx::PxRigidDynamic*)pairHeader.actors[1]];

		if (gameObject1 && gameObject2)
		{
			if (cPair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				LOG_BOTH("JUST ENTERED THE CONTACT");
			}
			else if (cPair.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
			}
			else if (cPair.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				LOG_BOTH("JUST LEAVED THE CONTACT");
			}
		}
	}
}

void PhysxSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
}
