#ifndef __P_WORLD_H__
#define __P_WORLD_H__
#include <vector>
#include "MathGeoLib/Math/float3.h"
#include <functional>
struct P_Actor;
struct CollisionActor;
struct RigidBody;
struct Collision;
class P_Solver;


class CollisionWorld
{
public:
	void AddCollisionActor(CollisionActor* actor);
	void RemoveCollisionActor(CollisionActor* actor);

	void AddSolver(P_Solver* solver);
	void RemoveSolver(P_Solver* solver);

	void SetCollisionCallback(std::function<void(Collision&, float)>& callback);

	void SolveCollisions(std::vector<Collision>& collisions, float dt);

	void SendCollisionCallbacks(std::vector<Collision>& collisions, float dt);

	void ResolveCollisions(float dt);
protected:
	std::vector <CollisionActor*> actors;
	std::vector <P_Solver*> solvers;
	std::function<void(Collision&, float)> onCollision;

};

class DynamicsWorld : public CollisionWorld
{
private:
	float3 gravity = float3(0, 0.0f, 0);
public:
	void AddRigidBody(RigidBody* rigidBody);
	void ApplyGravity();
	void MoveObjects(float dt);
	void Step(float dt);
};
#endif // !__P_WORLD_H__

