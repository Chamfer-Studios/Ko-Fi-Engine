#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include <string>
#include "Globals.h"
#include "ParticleModule.h"
#include "Texture.h"

class Texture;
class ParticleModule;
class EmitterDefault;
class EmitterMovement;
class EmitterColor;
class EmitterSize;

class Emitter
{
public:
	Emitter();
	~Emitter();

	bool Update(float dt);
	bool Cleanup();

	template<class T> T* CreateModule()
	{
		T* newModule = new T();
		return newModule;
	}

	template<class T> T* GetModule()
	{
		T* eModule = nullptr;
		for (ParticleModule* c : modules)
		{
			eModule = dynamic_cast<T*>(c);
			if (eModule)
				break;
		}
		return eModule;
	}

	void AddModule(ParticleModule* m);
	void DeleteModule(ParticleModule* m);

public:
	uint maxParticles = 50;
	std::vector<ParticleModule*> modules;
	std::string name = "Particle Emitter";
	Texture* texture = nullptr;
	std::string path = "none";
};

#endif // !__EMITTER_H__