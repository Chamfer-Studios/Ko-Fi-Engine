#ifndef __PARTICLE_RESOURCE_H__
#define __PARTICLE_RESOURCE_H__

#include "Emitter.h"
#include "Resource.h"

class R_ParticleResource : public Resource
{
public:
	R_ParticleResource(const char* resourceName = "New Resource");
	~R_ParticleResource();

	bool CleanUp();

public:
	std::vector<Emitter*> emitters;
	std::string name;
};

#endif // !__PARTICLE_RESOURCE_H__