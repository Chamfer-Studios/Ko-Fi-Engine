#include "Physics.h"

#if defined _DEBUG
#pragma comment(lib, "PhysDebug\PhysX_32.lib")
#pragma comment(lib, "PhysDebug\PhysXCommon_32.lib")
#pragma comment(lib, "PhysDebug\PhysXCooking_32.lib")
#pragma comment(lib, "PhysDebug\PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysDebug\PhysXFoundation_32.lib")
#pragma comment(lib, "PhysDebug\PhysXPvdSDK_static_32.lib")
#else
#pragma comment(lib, "PhysRelease\PhysX_32.lib")
#pragma comment(lib, "PhysRelease\PhysXCommon_32.lib")
#pragma comment(lib, "PhysRelease\PhysXCooking_32.lib")
#pragma comment(lib, "PhysRelease\PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysRelease\PhysXFoundation_32.lib")
#pragma comment(lib, "PhysRelease\PhysXPvdSDK_static_32.lib")
#endif // _DEBUG

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
