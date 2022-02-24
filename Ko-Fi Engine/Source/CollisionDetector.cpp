#include "CollisionDetector.h"

#include "Globals.h"
#include "Engine.h"

#include "SceneManager.h"
#include "ComponentMesh.h"
#include "ComponentCollider.h"

#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"


CollisionDetector::CollisionDetector(KoFiEngine* engine)
{
	this->engine = engine;
}

CollisionDetector::~CollisionDetector()
{
}

bool CollisionDetector::Update()
{
	//timed updates?
	for (int i = 0; i < collidableEntities.size(); i++)
	{
		CheckCollisions(collidableEntities[i]);
	}

	return false;
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
	math::AABB currentEntityBB = currentEntity->GetComponent<ComponentMesh>()->GetGlobalAABB();
	
	for (int i = 0; i < fullGOList.size(); i++)
	{
		if (fullGOList[i]->GetComponent<ComponentMesh>() != nullptr && fullGOList[i]->GetComponent<ComponentCollider>() != nullptr)
		{
			if (fullGOList[i] == currentEntity)
			{
				continue;
			}
			math::AABB newCollider = currentEntity->GetComponent<ComponentMesh>()->GetGlobalAABB();
			ColliderType newColliderType = fullGOList[i]->GetComponent<ComponentCollider>()->GetColliderType();

			if (currentEntityBB.Intersects(newCollider))
			{
				//call corresponding event depending on collider type

				switch (newColliderType)
				{
				case ColliderType::FLOOR:
				{
					break;
				}
				case ColliderType::WALL:
				{
					break;
				}
				case ColliderType::ENEMY:
				{
					break;
				}
				case ColliderType::PLAYER:
				{
					break;
				}
				}
			}
		}
	}
}
