#ifndef __MODULE_PHYSICS_H__
#define __MODULE_PHYSICS_H__

#include "Module.h"
#include <vector>
#include <string>
#include "Globals.h"

class GameObject;

class Physics : public Module
{
public:
	Physics(KoFiEngine* engine); // Module constructor
	~Physics(); // Module destructor

	bool Awake(Json configModule);
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();




private:
	KoFiEngine* engine = nullptr;
};

#endif // !__MODULE_PHYSICS_H__

