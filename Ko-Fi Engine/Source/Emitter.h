#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include <string>
#include "Globals.h"
#include "R_Texture.h"

enum class ParticleModuleType;
class ParticleModule;
class EmitterDefault;
class EmitterMovement;
class EmitterColor;
class EmitterSize;
class ParticleBillboarding;
//class R_Texture;

#define MAX_PARTICLES 500

class Emitter
{
public:
	Emitter(const char* name = "Emitter");
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
	void AddModuleByType(ParticleModuleType type);
	void DeleteModule(ParticleModule* m);

public:
	uint maxParticles = MAX_PARTICLES;
	uint particlesPerSpawn = 1;
	std::vector<ParticleModule*> modules;
	std::string name = "Emitter";
	R_Texture* texture = nullptr;
	bool checkerTexture = false;
};

#endif // !__EMITTER_H__