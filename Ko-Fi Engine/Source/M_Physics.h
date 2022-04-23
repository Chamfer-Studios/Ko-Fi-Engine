#ifndef __M_PHYSICS_H__
#define __M_PHYSICS_H__
#include "Module.h"
#include "Globals.h"
#include "reactphysics3d/reactphysics3d.h"

class M_Physics : public Module
{
public:
	//Constructor
	M_Physics(KoFiEngine* engine);

	//Destructor
	~M_Physics();

	//Called before render is avilable
	bool Awake(Json configModule);
	bool Start();

	bool Update(float dt);

	//Called before quitting
	bool CleanUp();

	//Engine Config Serialization ----------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	//---------------------------------------------------------------
	
	//Engine Config Inspector draw ----------------------------------
	bool InspectorDraw() override;
	// --------------------------------------------------------------

	// Method to receive and manage events
	void OnNotify(const Event& event);

	//Getters and Setters
	inline reactphysics3d::PhysicsWorld* GetWorld() { return this->world; }
	inline reactphysics3d::PhysicsCommon& GetPhysicsCommon() { return this->physicsCommon; }

private:
	KoFiEngine* engine = nullptr;
public:
	//DynamicsWorld* world = nullptr;

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = nullptr;

};

#endif // !__M_PHYSICS_H__
