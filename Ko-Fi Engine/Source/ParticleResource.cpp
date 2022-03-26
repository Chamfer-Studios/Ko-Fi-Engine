#include "ParticleResource.h"

ParticleResource::ParticleResource(const char* resourceName)
{
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	name = resourceName;
}

ParticleResource::~ParticleResource()
{
	if (emitters.size() > 0)
	{
		for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
		{
			it = emitters.erase(it);
		}
	}
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
}

bool ParticleResource::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	{
		it = emitters.erase(it);
	}
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	return true;
}