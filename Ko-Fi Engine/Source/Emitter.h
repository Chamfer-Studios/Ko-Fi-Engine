#ifndef __EMITTER_H__
#define __EMITTER_H__

#include <vector>
#include <string>
#include "Globals.h"
#include "Texture.h"

enum class ParticleModuleType;
class ParticleModule;
class EmitterDefault;
class EmitterMovement;
class EmitterColor;
class EmitterSize;
//class Texture;

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
	uint maxParticles = 50;
	std::vector<ParticleModule*> modules;
	std::string name = "Emitter";
	Texture texture;
};

#endif // !__EMITTER_H__