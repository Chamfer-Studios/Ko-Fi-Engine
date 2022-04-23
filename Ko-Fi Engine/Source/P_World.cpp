//#include "P_World.h"
//#include "Globals.h"
//#include "P_Actor.h"
//#include "P_Collision.h"
//#include "P_Solver.h"
//
//void CollisionWorld::AddCollisionActor(CollisionActor* actor)
//{
//	actors.push_back(actor);
//}
//
//void CollisionWorld::RemoveCollisionActor(CollisionActor* actor)
//{
//	auto position = std::find(actors.begin(), actors.end(), actor);
//
//	actors.erase(position);
//	RELEASE(actor);
//}
//
//void CollisionWorld::AddSolver(P_Solver* solver)
//{
//	solvers.push_back(solver);
//}
//
//void CollisionWorld::RemoveSolver(P_Solver* solver)
//{
//	auto position = std::find(solvers.begin(), solvers.end(), solver);
//
//	solvers.erase(position);
//	RELEASE(solver);
//}
//
//void CollisionWorld::SetCollisionCallback(std::function<void(Collision&, float)>& callback)
//{
//	this->onCollision = callback;
//}
//
//void CollisionWorld::SolveCollisions(std::vector<Collision>& collisions, float dt)
//{
//	for (P_Solver* solver : solvers) {
//		solver->Solve(collisions, dt);
//	}
//}
//
//void CollisionWorld::SendCollisionCallbacks(std::vector<Collision>& collisions, float dt)
//{
//	for (Collision& collision : collisions)
//	{
//		this->onCollision(collision, dt);
//		const auto& a = collision.objA->OnCollision();
//		const auto& b = collision.objB->OnCollision();
//		if (a) a(collision, dt);
//		if (b) b(collision, dt);
//	}
//}
//
//void CollisionWorld::ResolveCollisions(float dt)
//{
//	std::vector<Collision> collisions;
//	std::vector<Collision> triggers;
//	for (CollisionActor* a : actors)
//	{
//		for (CollisionActor* b : actors)
//		{
//			if (a == b)break;
//			if(!a->GetCollider() || !b->GetCollider())
//			{
//				continue;
//			}
//			if (a->GetCollider()->Intersects(a->GetCollider(),b->GetCollider()))
//			{
//				if (a->GetIsTrigger() || b->GetIsTrigger())
//				{
//					//triggers.emplace_back(a,b);
//				}
//				else {
//					//collisions.emplace_back(a,b);
//				}
//			}
//		}
//	}
//	SolveCollisions(collisions, dt);//don't solve triggers
//	SendCollisionCallbacks(collisions, dt);
//	SendCollisionCallbacks(triggers, dt);
//
//}
//
//void DynamicsWorld::AddRigidBody(RigidBody* rigidBody)
//{
//	if (rigidBody->GetTakesGravity())
//	{
//		rigidBody->SetGravity(gravity);
//	}
//	AddCollisionActor(rigidBody);
//}
//
//void DynamicsWorld::ApplyGravity()
//{
//	for (CollisionActor* actor : actors)
//	{
//		if (!actor->GetIsDynamic()) continue;
//		RigidBody* rigidBody = (RigidBody*)actor;
//		float3 acceleration = rigidBody->GetGravity() * rigidBody->GetMass();
//		rigidBody->SetForce(acceleration);
//	}
//}
//
//void DynamicsWorld::MoveObjects(float dt)
//{
//	for (CollisionActor* actor : actors)
//	{
//		RigidBody* rigidBody = (RigidBody*)actor;
//	
//		float3 vel = rigidBody->GetVelocity() + rigidBody->GetForce()/rigidBody->GetMass() * dt;
//		rigidBody->SetVelocity(vel);
//		float3 pos = rigidBody->GetTransform()->position + rigidBody->GetVelocity() * dt;
//		rigidBody->GetTransform()->position = pos;
//		rigidBody->SetForce(float3::zero);
//	}
//}
//
//void DynamicsWorld::Step(float dt)
//{
//	ApplyGravity();
//	ResolveCollisions(dt);
//	MoveObjects(dt);
//}
//
