#include "Physics.h"

#pragma comment (lib, "PhysX_32.lib")    
#pragma comment (lib, "PhysXCharacterKinematic_static_32.lib")
#pragma comment (lib, "PhysXCommon_32.lib")    
#pragma comment (lib, "PhysXCooking_32.lib") 
#pragma comment (lib, "PhysXExtensions_static_32.lib")    
#pragma comment (lib, "PhysXFoundation_32.lib") 
#pragma comment (lib, "PhysXPvdSDK_static_32.lib") 
#pragma comment (lib, "PhysXTask_static_32.lib")    
#pragma comment (lib, "PhysXVehicle_static_32.lib") 

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
