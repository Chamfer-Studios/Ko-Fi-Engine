#include "CollisionDetector.h"

#include "Globals.h"
#include "Engine.h"

#include "SceneManager.h"
#include "ComponentMesh.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"

#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"

#include <iostream>


CollisionDetector::CollisionDetector(KoFiEngine* engine)
{
	name = "CollisionDetector";
	this->engine = engine;
}

CollisionDetector::~CollisionDetector()
{
}

bool CollisionDetector::Update(float dt)
{
	//TODO: timed updates?

	for (int i = 0; i < collidableEntities.size(); i++)
	{
		CheckCollisions(collidableEntities[i]);
	}

	return true;
}

bool CollisionDetector::CleanUp()
{
	collidableEntities.clear();
	return true;
}

void CollisionDetector::AddCollidableEntity(GameObject* GO)
{
	for (int i = 0; i < collidableEntities.size(); i++)
	{
		if (collidableEntities[i] == GO)
		{
			//LOG(" CollisionDetector Error: Trying to add an GO that is already inside collidableEntities ");
			return;
		}
	}

	collidableEntities.push_back(GO);
}

void CollisionDetector::RemoveCollidableEntity(GameObject* GO)
{
	for (int i = 0; i < collidableEntities.size(); i++)
	{
		if (collidableEntities[i] == GO)
		{
			collidableEntities.erase(collidableEntities.begin() + i);
			return;
		}
	}

	//LOG(" CollisionDetector Error: Trying to remove an GO that is not inside collidableEntities ");
}

void CollisionDetector::CheckCollisions(GameObject* currentEntity)
{
	std::vector<GameObject*> fullGOList = engine->GetSceneManager()->GetCurrentScene()->gameObjectList;

	currentEntity->GetComponent<ComponentMesh>()->GenerateGlobalBoundingBox();
	math::AABB currentEntityAABB = currentEntity->GetComponent<ComponentMesh>()->GetGlobalAABB();
	ColliderType currentType = currentEntity->GetComponent<ComponentCollider>()->GetColliderType();

	for (int i = 0; i < fullGOList.size(); i++)
	{
		if (fullGOList[i]->GetComponent<ComponentMesh>() != nullptr && fullGOList[i]->GetComponent<ComponentCollider>() != nullptr)
		{

			if (fullGOList[i] == currentEntity)
			{
				continue;
			}
			
			math::AABB newCollider = fullGOList[i]->GetComponent<ComponentMesh>()->GetGlobalAABB();
			ColliderType newColliderType = fullGOList[i]->GetComponent<ComponentCollider>()->GetColliderType();

			/*if (a.MinX() <= b.MaxX() && a.MaxX() >= b.MinX() &&
				(a.MinY() <= b.MaxY() && a.MaxY() >= b.MinY()) &&
				(a.MinZ() <= b.MaxZ() && a.MaxZ() >= b.MinZ()))
			*/

			if (currentEntityAABB.Intersects(newCollider))
			{
				//call corresponding event depending on collider type
				//std::cout<<"intersectioooooon";

				switch (newColliderType)
				{
				case ColliderType::FLOOR:
				{
					float3 currentVelocity = currentEntity->GetComponent<ComponentRigidBody>()->GetLinearVelocity();
					currentEntity->GetComponent<ComponentRigidBody>()->SetLinearVelocity(float3(currentVelocity.x, -currentVelocity.y, currentVelocity.z)); //change velocity direction excluding y axis.

					break;
				}
				case ColliderType::WALL:
				{
					float3 currentVelocity = currentEntity->GetComponent<ComponentRigidBody>()->GetLinearVelocity();
					currentEntity->GetComponent<ComponentRigidBody>()->SetLinearVelocity(float3( -currentVelocity.x, currentVelocity.y, -currentVelocity.z)); //change velocity direction excluding y axis.

					break;
				}
				case ColliderType::ENEMY:
				{
					float3 currentVelocity = currentEntity->GetComponent<ComponentRigidBody>()->GetLinearVelocity();
					currentEntity->GetComponent<ComponentRigidBody>()->SetLinearVelocity(float3(-currentVelocity.x, currentVelocity.y, -currentVelocity.z)); //change velocity direction excluding y axis.

					break;
				}
				case ColliderType::PLAYER:
				{
					float3 currentVelocity = currentEntity->GetComponent<ComponentRigidBody>()->GetLinearVelocity();
					currentEntity->GetComponent<ComponentRigidBody>()->SetLinearVelocity(float3(-currentVelocity.x, currentVelocity.y, -currentVelocity.z)); //change velocity direction excluding y axis.

					break;
				}
				}
			}
		}
	}
}
