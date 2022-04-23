#include "M_Physics.h"
M_Physics::M_Physics(KoFiEngine* engine)
{
	this->engine = engine;
	reactphysics3d::PhysicsCommon physicsCommon;
	world = physicsCommon.createPhysicsWorld();

	//this->world = new DynamicsWorld();
}

M_Physics::~M_Physics()
{
}

bool M_Physics::Awake(Json configModule)
{
	


	return true;
}

bool M_Physics::Start()
{
	reactphysics3d::Vector3 position(0, 20, 0);
	reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
	reactphysics3d::Transform transform(position, orientation);
	body = world->createRigidBody(transform);
	world->setIsDebugRenderingEnabled(false);
	return true;
}

bool M_Physics::Update(float dt)
{
	//this->world->Step(dt);
	world->update(dt);
	// Get the updated position of the body 
	const reactphysics3d::Transform& transform = body->getTransform();
	const reactphysics3d::Vector3& position = transform.getPosition();

	// Display the position of the body 
	std::cout << "Body Position: (" << position.x << ", " <<
		position.y << ", " << position.z << ")" << std::endl;
	return true;
}

bool M_Physics::CleanUp()
{
	return true;
}

bool M_Physics::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Physics::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Physics::InspectorDraw()
{
	return true;
}

void M_Physics::OnNotify(const Event& event)
{
}
