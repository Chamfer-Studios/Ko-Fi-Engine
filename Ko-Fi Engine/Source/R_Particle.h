#ifndef __R_PARTICLE_H__
#define __R_PARTICLE_H__

#include "Emitter.h"
#include "Resource.h"

class R_Particle : public Resource
{
public:
	R_Particle(const char* resourceName = "New Particle Effect");
	~R_Particle();

	bool CleanUp();

public:
	std::vector<Emitter*> emitters;
	std::string name;
};

#endif // !__R_PARTICLE_H__