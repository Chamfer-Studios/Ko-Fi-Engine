#ifndef __PARTICLE_RESOURCE_H__
#define __PARTICLE_RESOURCE_H__

#include "Emitter.h"

class ParticleResource
{
public:
	ParticleResource();
	~ParticleResource();

	bool CleanUp();

public:
	std::vector<Emitter*> emitters;
};

#endif // !__PARTICLE_RESOURCE_H__