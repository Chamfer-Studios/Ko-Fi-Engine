#include "M_Physics.h"
#include "P_World.h"

M_Physics::M_Physics(KoFiEngine* engine)
{
	this->engine = engine;
	this->world = new DynamicsWorld();
}

M_Physics::~M_Physics()
{
}

bool M_Physics::Awake(Json configModule)
{
	return true;
}

bool M_Physics::Update(float dt)
{
	this->world->Step(dt);
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
