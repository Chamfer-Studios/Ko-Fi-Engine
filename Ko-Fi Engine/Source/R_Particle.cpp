#include "R_Particle.h"

R_Particle::R_Particle(const char* resourceName) : Resource(ResourceType::PARTICLE)
{
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	name = resourceName;
}

R_Particle::~R_Particle()
{
	CleanUp();
}

bool R_Particle::CleanUp()
{
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
	{
		emitters.erase(it);
		delete *it;
		if (emitters.empty())
			break;
	}
	emitters.clear();
	emitters.shrink_to_fit();
	name.clear();
	return true;
}