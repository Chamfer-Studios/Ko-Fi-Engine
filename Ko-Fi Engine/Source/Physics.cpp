#include "Physics.h"

// Module constructor
Physics::Physics(KoFiEngine* engine)
{
	name = "Physics";
	this->engine = engine;
}

// Module destructor
Physics::~Physics()
{
}

bool Physics::Start()
{

	return true;
}

bool Physics::Update(float dt)
{
	return true;
}

bool Physics::CleanUp()
{
	return true;
}

void Physics::OnGui()
{
}
