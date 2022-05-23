#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__

#include <vector>
#include "Globals.h"
#include "C_Particle.h"
#include "Particle.h"
#include "Emitter.h"

class C_Particle;
class Emitter;
class Particle;

class EmitterInstance
{
public:
	EmitterInstance(Emitter* e, C_Particle* cp);
	~EmitterInstance();
	
	void Init();
	bool Update(float dt);

	//change burst to generate more than particle at once
	void SpawnParticle(int burst = 1);

	void KillDeadParticles();
	void KillAllParticles();
	void SetParticleEmission(bool set);
	bool GetParticleEmission();

public:
	uint activeParticles = 0;
	std::vector<Particle> particles;
	unsigned int* particleIndices = nullptr;

	float emitterLife = 0.0f;
	bool loop = true;

	Emitter* emitter = nullptr;
	C_Particle* component = nullptr;

private:
	bool deactivateParticleEmission = false;
};

#endif // !__EMITTER_INSTANCE_H__