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

void CollisionDetector::OnNotify(const Event& event)
{
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
				std::cout<<"intersectioooooon";

				//set new collision for the inspector properties
				currentEntity->GetComponent<ComponentCollider>()->SetNewCollision(fullGOList[i]);
				fullGOList[i]->GetComponent<ComponentCollider>()->SetNewCollision(currentEntity);

				//basic rejection calculations
				math::float3 center1 = currentEntityAABB.CenterPoint();
				math::float3 center2 = newCollider.CenterPoint();

				math::float3 Size1 = currentEntityAABB.Size();
				math::float3 Size2 = newCollider.Size();

				math::float3 initialDistance = center1 - center2;

				//TODO: hay que tener en cuenta la direccion de la que viene la colision para tenerla en cuenta a la hora de hacer los calculos
				math::float2 finalDistanceXZ = math::float2((Size1.x/2 + Size2.x/2),(Size1.z/2 + Size2.z/2));
				math::float3 wallCollisionTranslation = math::float3(finalDistanceXZ.x, 0, finalDistanceXZ.y) - math::float3(initialDistance.x, 0, initialDistance.z);

				switch (newColliderType)
				{
				case ColliderType::FLOOR:
				{
					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						float finalDistanceY = math::Abs(Size1.y) / 2 + math::Abs(Size2.y) / 2;
						float floorCollisionTranslation = finalDistanceY - initialDistance.y;

						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + float3(0, floorCollisionTranslation, 0);
						currentEntity->GetTransform()->SetPosition(finalPosition);
						
						//eliminar velocidad en x
					}
					break;
				}
				case ColliderType::WALL:
				{	
					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						//TODO: hay que tener en cuenta la direccion de la que viene la colision para tenerla en cuenta a la hora de hacer los calculos
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
					}
					break;
				}
				case ColliderType::ENEMY:
				{
					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
					}

					break;
				}
				case ColliderType::PLAYER:
				{
					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
					}
					break;
				}
				}
			}
			//if they are not colliding anymore, set the isColliding bool to false
			else if (currentEntity->GetComponent<ComponentCollider>()->IsColliding() && currentEntity->GetComponent<ComponentCollider>()->GetLastCollided() == fullGOList[i])
			{
				currentEntity->GetComponent<ComponentCollider>()->SetColliding(false);
				fullGOList[i]->GetComponent<ComponentCollider>()->SetColliding(false);
			}
		}
	}
}
