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

bool CollisionDetector::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
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

	if (currentEntity->GetComponent<ComponentMesh>() == nullptr) return;
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

			if (currentEntityAABB.Intersects(newCollider))
			{
				//call corresponding event depending on collider type
				//std::cout<<"intersectioooooon";

				//set new collision for the inspector properties
				currentEntity->GetComponent<ComponentCollider>()->SetNewCollision(fullGOList[i]);
				fullGOList[i]->GetComponent<ComponentCollider>()->SetNewCollision(currentEntity);

				//basic rejection calculations
				math::float3 center1 = currentEntityAABB.CenterPoint();
				math::float3 center2 = newCollider.CenterPoint();

				math::float3 Size1 = currentEntityAABB.Size();
				math::float3 Size2 = newCollider.Size();

				math::float3 initialDistance = center1 - center2;

				switch (newColliderType)
				{
				case ColliderType::FLOOR:
				{
					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						float finalDistanceY = math::Abs(Size1.y) / 2 + math::Abs(Size2.y) / 2;
						float floorCollisionTranslation = finalDistanceY - math::Abs(initialDistance.y);

						//Move currentEntity out of the floor
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + float3(0, floorCollisionTranslation, 0);
						currentEntity->GetTransform()->SetPosition(finalPosition);
						currentEntity->GetComponent<ComponentRigidBody>()->TransformUpdatesRigidBody();
						
						//Stop gravity
						currentEntity->GetComponent<ComponentRigidBody>()->SetUseGravity(false);

						//lock Y axis at desired position
						currentEntity->GetComponent<ComponentRigidBody>()->FreezePositionY(true);
					}
					break;
				}
				case ColliderType::WALL:
				{	
					bool leftCollision = false;
					bool frontCollision = false;

					if (currentEntity->GetComponent<ComponentRigidBody>())
					{
 						math::float2 finalDistanceXZ = math::float2((Size1.x / 2 + Size2.x / 2), (Size1.z / 2 + Size2.z / 2));
						math::float3 wallCollisionTranslation = math::float3(finalDistanceXZ.x, 0, finalDistanceXZ.y) - math::float3(math::Abs(initialDistance.x), 0, math::Abs(initialDistance.z));

						if (center2.x > center1.x)	//collision comming from the left
							leftCollision = true;	//translation.z negative
						else						//collision comming from the right
							leftCollision = false;	//translation.z positive

						if (center2.z > center1.z)	//collision comming from the front
							frontCollision = true;	//translation.z negative
						else						//collision comming from the back
							frontCollision = false; //translation.z positive

						if (math::Abs(wallCollisionTranslation.x) < math::Abs(wallCollisionTranslation.z))
						{
							if(leftCollision)
								wallCollisionTranslation = math::float3(-(math::Abs(wallCollisionTranslation.x)), .0f, .0f);
							else
								wallCollisionTranslation = math::float3(((math::Abs(wallCollisionTranslation.x))), .0f, .0f);
						}
						else
						{
							if(frontCollision)
								wallCollisionTranslation = math::float3(0.f, .0f, (-(math::Abs(wallCollisionTranslation.z))));
							else
								wallCollisionTranslation = math::float3(0.f, .0f, ((math::Abs(wallCollisionTranslation.z))));
						}

						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
						currentEntity->GetComponent<ComponentRigidBody>()->TransformUpdatesRigidBody();
					}
					break;
				}
				case ColliderType::ENEMY:
				{
					/*if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
					}*/

					break;
				}
				case ColliderType::PLAYER:
				{
					/*if (currentEntity->GetComponent<ComponentRigidBody>())
					{
						math::float3 finalPosition = currentEntity->GetTransform()->GetPosition() + wallCollisionTranslation;
						currentEntity->GetTransform()->SetPosition(finalPosition);
					}*/
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

bool CollisionDetector::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool CollisionDetector::LoadConfiguration(Json& configModule)
{
	return true;
}

bool CollisionDetector::InspectorDraw()
{
	return true;
}
