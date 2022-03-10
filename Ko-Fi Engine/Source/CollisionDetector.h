#pragma once

#include "Module.h"
#include <vector>

class GameObject;

class CollisionDetector : public Module
{
public:

	CollisionDetector(KoFiEngine* engine);
	~CollisionDetector();

	bool Update(float dt);
	bool CleanUp();
	void OnNotify(const Event& event);
	void AddCollidableEntity(GameObject* GO); //TODO: The GameObject should add itself here when a ComponentCollider is added.
	void RemoveCollidableEntity(GameObject* GO); //TODO: The GameObject should remove itself from the list at cleanup/when deactivated.

	void CheckCollisions(GameObject* currentEntity); //loop through all gameobjects that are not itself and check for intersection between AABBs

private:
	
	std::vector<GameObject*> collidableEntities;	//vector where collidable entities (player and enemies) are added to be checked for collisions.
													//still not sure if i have to store here GOs, their meshes, the AABBs or what. Since it is a pointer and takes the same space, i am using GO for the moment.

	KoFiEngine* engine = nullptr;
};