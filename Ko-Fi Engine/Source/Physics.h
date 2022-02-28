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

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnGui() override;
	void OnSave(JSONWriter& writer) const override;
	void OnLoad(const JSONReader& reader) override;

private:
	KoFiEngine* engine = nullptr;

	PxFoundation* foundation = nullptr;
};

#endif // !__MODULE_PHYSICS_H__

