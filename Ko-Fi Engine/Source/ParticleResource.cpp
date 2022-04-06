#include "ParticleResource.h"

ParticleResource::ParticleResource(const char* resourceName)
{
	emitters.clear();
	name.clear();
	name = resourceName;
}

ParticleResource::~ParticleResource()
{
	
}

bool ParticleResource::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
	{
		emitters.erase(it);
		if (emitters.empty())
			break;
	}
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	return true;
}