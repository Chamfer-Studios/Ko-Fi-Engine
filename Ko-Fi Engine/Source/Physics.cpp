#include "Physics.h"

#if defined _DEBUG
#pragma comment(lib, "PhysX_32.lib")
#pragma comment(lib, "PhysXCommon_32.lib")
#pragma comment(lib, "PhysXCooking_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysXFoundation_32.lib")
#pragma comment(lib, "PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
#else
#pragma comment(lib, "PhysX_32.lib")
#pragma comment(lib, "PhysXCommon_32.lib")
#pragma comment(lib, "PhysXCooking_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysXFoundation_32.lib")
#pragma comment(lib, "PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
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
