#ifndef __I_PARTICLES_H__
#define __I_PARTICLES_H__
#include <string>

class R_Particle;
class KoFiEngine;

class I_Particle
{
public:
	I_Particle(KoFiEngine* engine);
	~I_Particle();

	bool Create(R_Particle* particle, bool loop);
	bool Save(const R_Particle* particle, const char* path);
	bool Load(R_Particle* particle, const char* name, int loop);

private:
	KoFiEngine* engine = nullptr;
};

#endif // !__I_PARTICLES_H__