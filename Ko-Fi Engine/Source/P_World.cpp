#include "P_World.h"
#include "Globals.h"
#include "P_Actor.h"
#include "P_Collision.h"
#include "P_Solver.h"


//void P_World::ResolveCollisions(float dt)
//{
//	std::vector<Collision> collisions;
//	for (P_Actor* a : actors) {
//		for (P_Actor* b : actors) {
//			if (a == b) break;
//
//			if (!a->collider
//				|| !b->collider)
//			{
//				continue;
//			}
//
//			CollisionPoints points = a->collider->TestCollision(
//				a->transform,
//				b->collider,
//				b->transform);
//
//			if (points.hasCollision) {
//				collisions.emplace_back(a, b, points);
//			}
//		}
//	}
//	for (P_Solver* solver : solvers)
//	{
//		solver->Solve(collisions, dt);
//	}
//
//}

void CollisionWorld::AddCollisionActor(CollisionActor* actor)
{
	actors.push_back(actor);
}

void CollisionWorld::RemoveCollisionActor(CollisionActor* actor)
{
	auto position = std::find(actors.begin(), actors.end(), actor);

	actors.erase(position);
	RELEASE(actor);
}

void CollisionWorld::AddSolver(P_Solver* solver)
{
	solvers.push_back(solver);
}

void CollisionWorld::RemoveSolver(P_Solver* solver)
{
	auto position = std::find(solvers.begin(), solvers.end(), solver);

	solvers.erase(position);
	RELEASE(solver);
}

void CollisionWorld::SetCollisionCallback(std::function<void(Collision&, float)>& callback)
{
}

void CollisionWorld::SolveCollisions(std::vector<Collision>& collisions, float dt)
{
}

void CollisionWorld::SendCollisionCallbacks(std::vector<Collision>& collisions, float dt)
{
}

void CollisionWorld::Step(float dt)
{
}

void CollisionWorld::ResolveCollisions(float dt)
{
}

void DynamicsWorld::AddRigidBody(RigidBody* rigidBody)
{
	if (rigidBody->GetTakesGravity())
	{
		rigidBody->SetGravity(gravity);
	}
	AddCollisionActor(rigidBody);
}

void DynamicsWorld::ApplyGravity()
{
	for (CollisionActor* actor : actors)
	{
		if (!actor->GetIsDynamic()) continue;
		RigidBody* rigidBody = (RigidBody*)actor;
		float3 acceleration = rigidBody->GetGravity() * rigidBody->GetMass();
		rigidBody->SetForce(acceleration);
	}
}

void DynamicsWorld::MoveObjects(float dt)
{
	for (CollisionActor* actor : actors)
	{
		RigidBody* rigidBody = (RigidBody*)actor;
	
		float3 vel = rigidBody->GetVelocity() + rigidBody->GetForce()/rigidBody->GetMass() * dt;
		rigidBody->SetVelocity(vel);
		float3 pos = rigidBody->GetTransform()->position + rigidBody->GetVelocity() * dt;
		rigidBody->GetTransform()->position = pos;
		rigidBody->SetForce(float3::zero);
	}
}

void DynamicsWorld::Step(float dt)
{
	ApplyGravity();
	ResolveCollisions(dt);
	MoveObjects(dt);
}

