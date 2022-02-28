#include "Physics.h"

#ifndef NDEBUG
#pragma comment (lib, "PhysRelease/PhysX_32.lib")    
#pragma comment (lib, "PhysRelease/PhysXCharacterKinematic_static_32.lib")
#pragma comment (lib, "PhysRelease/PhysXCommon_32.lib")    
#pragma comment (lib, "PhysRelease/PhysXCooking_32.lib") 
#pragma comment (lib, "PhysRelease/PhysXExtensions_static_32.lib")    
#pragma comment (lib, "PhysRelease/PhysXFoundation_32.lib") 
#pragma comment (lib, "PhysRelease/PhysXPvdSDK_static_32.lib") 
#pragma comment (lib, "PhysRelease/PhysXTask_static_32.lib")    
#pragma comment (lib, "PhysRelease/PhysXVehicle_static_32.lib") 
#else
#pragma comment (lib, "PhysDebug/PhysX_32.lib")    
#pragma comment (lib, "PhysDebug/PhysXCharacterKinematic_static_32.lib")
#pragma comment (lib, "PhysDebug/PhysXCommon_32.lib")    
#pragma comment (lib, "PhysDebug/PhysXCooking_32.lib") 
#pragma comment (lib, "PhysDebug/PhysXExtensions_static_32.lib")    
#pragma comment (lib, "PhysDebug/PhysXFoundation_32.lib") 
#pragma comment (lib, "PhysDebug/PhysXPvdSDK_static_32.lib") 
#pragma comment (lib, "PhysDebug/PhysXTask_static_32.lib")    
#pragma comment (lib, "PhysDebug/PhysXVehicle_static_32.lib") 
#endif // !NDEBUG



// Module constructor
Physics::Physics(KoFiEngine* engine) : Module()
{
	name = "Physics";
	this->engine = engine;
}

// Module destructor
Physics::~Physics()
{
}

bool Physics::Awake(Json configModule)
{
	return true;
}

bool Physics::Start()
{
	LOG_BOTH("Initializing PhysX 4.1 --------------------------------------------------------");



	LOG_BOTH("Finished to initialize PhysX 4.1 ----------------------------------------------");
	return true;
}

bool Physics::PreUpdate(float dt)
{
	return true;
}

bool Physics::Update(float dt)
{
	return true;
}

bool Physics::PostUpdate(float dt)
{
	return true;
}

bool Physics::CleanUp()
{
	return true;
}
