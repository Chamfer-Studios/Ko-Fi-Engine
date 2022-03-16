#include "ParticleResource.h"

ParticleResource::ParticleResource()
{
	emitters.clear();
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
}

bool ParticleResource::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	{
		it = emitters.erase(it);
	}
	return true;
}