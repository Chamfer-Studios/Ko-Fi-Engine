#include "ParticleResource.h"

ParticleResource::ParticleResource()
{
	emitters.clear();
	name.clear();
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
	name.clear();
}

bool ParticleResource::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	{
		it = emitters.erase(it);
	}
	name.clear();
	return true;
}