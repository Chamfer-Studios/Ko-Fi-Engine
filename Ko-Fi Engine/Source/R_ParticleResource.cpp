#include "R_ParticleResource.h"

R_ParticleResource::R_ParticleResource(const char* resourceName) : Resource(ResourceType::PARTICLE)
{
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	name = resourceName;
}

R_ParticleResource::~R_ParticleResource()
{
	CleanUp();
}

bool R_ParticleResource::CleanUp()
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