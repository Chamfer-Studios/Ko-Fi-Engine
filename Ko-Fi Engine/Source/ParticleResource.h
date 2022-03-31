#ifndef __PARTICLE_RESOURCE_H__
#define __PARTICLE_RESOURCE_H__

#include "Emitter.h"
#include "Resource.h"

class ParticleResource : public Resource
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