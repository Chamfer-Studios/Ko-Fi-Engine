#include "SimulationEventCallback.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_Script.h"
#include "C_Collider.h"
#include "PxSimulationEventCallback.h"
#include "Globals.h"
#include "Scripting.h"

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
			const std::string* fil1 = gameObject1->GetComponent<C_Collider>()->GetFilter();
			const std::string* fil2 = gameObject2->GetComponent<C_Collider>()->GetFilter();
			int fil1pos = callback->GetFilterID(fil1);
			int fil2pos = callback->GetFilterID(fil2);

			bool** filMatrix = callback->GetFilterMatrix();
			if (filMatrix[fil1pos][fil2pos])
			{
				C_Script* cScript1 = gameObject1->GetComponent<C_Script>();
				C_Script* cScript2 = gameObject2->GetComponent<C_Script>();
				if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					if (cScript1) {
						
							cScript1->s->handler->lua["OnCollisionEnter"](gameObject2);
						
					}
					if (cScript2) {
						
							cScript2->s->handler->lua["OnCollisionEnter"](gameObject1);
						
					}
				}
				else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
				{
					if (cScript1) {
						
							cScript1->s->handler->lua["OnCollisionRepeat"](gameObject2);
						
					}
					if (cScript2) {
						
							cScript2->s->handler->lua["OnCollisionRepeat"](gameObject1);
						
					}
				}
				else if (contactPairs.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					if (cScript1) {
						
							cScript1->s->handler->lua["OnCollisionExit"](gameObject2);
						
					}
					if (cScript2) {
						
							cScript2->s->handler->lua["OnCollisionExit"](gameObject1);
						
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
			const std::string* fil1 = gameObject1->GetComponent<C_Collider>()->GetFilter();
			const std::string* fil2 = gameObject2->GetComponent<C_Collider>()->GetFilter();
			int fil1pos = callback->GetFilterID(fil1);
			int fil2pos = callback->GetFilterID(fil2);

			bool** filMatrix = callback->GetFilterMatrix();
			if (filMatrix[fil1pos][fil2pos])
			{
				C_Script* cScript1 = gameObject1->GetComponent<C_Script>();
				C_Script* cScript2 = gameObject2->GetComponent<C_Script>();
				if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					if (cScript1) {
						
							cScript1->s->handler->lua["OnTriggerEnter"](gameObject2);
						
					}
					if (cScript2) {
						
							cScript2->s->handler->lua["OnTriggerEnter"](gameObject1);
						
					}
				}
				else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					if (cScript1) {
						
							cScript1->s->handler->lua["OnTriggerExit"](gameObject2);
						
					}
					if (cScript2) {
						
							cScript2->s->handler->lua["OnTriggerExit"](gameObject1);
						
					}
				}
			}
		}
	}
}