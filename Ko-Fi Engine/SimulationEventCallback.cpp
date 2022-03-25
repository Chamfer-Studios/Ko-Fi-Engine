#include "Source/SimulationEventCallback.h"
#include "Source/GameObject.h"
#include "Source/Physics.h"
#include "Source/ComponentScript.h"
#include "PhysX_4.1/include/PxSimulationEventCallback.h"
#include "Source/Globals.h"

void SimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; ++i)
	{
		const physx::PxContactPair& contactPairs = pairs[i];

		GameObject* gameObject1 = nullptr;
		GameObject* gameObject2 = nullptr;
		gameObject1 = callback->GetActors()[(physx::PxRigidDynamic*)pairHeader.actors[0]];
		gameObject2 = callback->GetActors()[(physx::PxRigidDynamic*)pairHeader.actors[1]];

		if (gameObject1 && gameObject2)
		{
			// For gameObject1
			for (int i = 0; i < gameObject1->GetComponents().size(); ++i)
			{
				if (gameObject1->GetComponent<ComponentScript>())
				{
					if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						LOG_BOTH("holi");
						// Call to OnCollisionEnter(gameObject2) to the scripting function
					}
					else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
					{
						// Call to OnCollisionRepeat(gameObject2) to the scripting function
					}
					else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						// Call to OnCollisionExit(gameObject2) to the scripting function
					}
				}
			}
			// For gameObject2
			for (int i = 0; i < gameObject2->GetComponents().size(); ++i)
			{
				if (gameObject2->GetComponent<ComponentScript>())
				{
					if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						// Call to OnCollisionEnter(gameObject1) to the scripting function
					}
					else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
					{
						// Call to OnCollisionRepeat(gameObject1) to the scripting function
					}
					else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						// Call to OnCollisionExit(gameObject1) to the scripting function
					}
				}
			}
		}
	}
}

void SimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; ++i)
	{
		GameObject* gameObject1 = nullptr;
		GameObject* gameObject2 = nullptr;
		gameObject1 = callback->GetActors()[(physx::PxRigidDynamic*)pairs[i].triggerActor];
		gameObject2 = callback->GetActors()[(physx::PxRigidDynamic*)pairs[i].otherActor];

		if (gameObject1 && gameObject2)
		{
			// ---------------------------------------------------------------------------------------------------
			// NOTE: I don't know if we would need an OnTriggerRepeat function, in case we need it I kept it there
			// ---------------------------------------------------------------------------------------------------

			// For gameObject1
			for (int i = 0; i < gameObject1->GetComponents().size(); ++i)
			{
				if (gameObject1->GetComponent<ComponentScript>())
				{
					if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						// Call to OnTriggerEnter(gameObject2) to the scripting function
					}
					else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
					{
						// Call to OnTriggerRepeat(gameObject2) to the scripting function
					}
					else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						// Call to OnTriggerExit(gameObject2) to the scripting function
					}
				}
			}
			// For gameObject2
			for (int i = 0; i < gameObject2->GetComponents().size(); ++i)
			{
				if (gameObject2->GetComponent<ComponentScript>())
				{
					if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						// Call to OnTriggerEnter(gameObject1) to the scripting function
					}
					else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
					{
						// Call to OnTriggerRepeat(gameObject1) to the scripting function
					}
					else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						// Call to OnTriggerExit(gameObject1) to the scripting function
					}
				}
			}
		}
	}
}
