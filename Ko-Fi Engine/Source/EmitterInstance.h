#ifndef __EMITTER_INSTANCE_H__
#define __EMITTER_INSTANCE_H__

#include <vector>
#include "Globals.h"
#include "ComponentParticle.h"
#include "Particle.h"
#include "Emitter.h"

class ComponentParticle;
class Emitter;
class Particle;

class EmitterInstance
{
public:
	EmitterInstance(Emitter* e, ComponentParticle* cp);
	~EmitterInstance();
	
	void Init(Emitter* e, ComponentParticle* cp);
	bool Update(float dt);

	void DrawParticles();

	void SpawnParticle();

	void KillDeadParticles();
	void KillParticles();
	void SetParticleEmission(bool set);

public:
	uint activeParticles = 0;
	std::vector<Particle> particles;
	unsigned int* particleIndices = nullptr;

	float emitterLife = 0.0f;

	Emitter* emitter = nullptr;
	ComponentParticle* component = nullptr;

private:
	bool deactivateParticleEmission = false;
};

#endif // !__EMITTER_INSTANCE_H__