#ifndef __PARTICLE_RESOURCE_H__
#define __PARTICLE_RESOURCE_H__

#include "Emitter.h"

class ParticleResource
{
public:
	ParticleResource(const char* resourceName = "New Resource");
	~ParticleResource();

	bool CleanUp();

public:
	std::vector<Emitter*> emitters;
	std::string name;
};

#endif // !__PARTICLE_RESOURCE_H__