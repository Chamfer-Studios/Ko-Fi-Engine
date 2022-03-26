#include "EmitterInstance.h"
#include "ParticleModule.h"
#include "Texture.h"

EmitterInstance::EmitterInstance(Emitter* e, ComponentParticle* cp) : emitter(e),component(cp)
{
	emitterLife = 0.0f;
	activeParticles = 0;
	particles.clear();
	particles.shrink_to_fit();
}

EmitterInstance::~EmitterInstance()
{}

void EmitterInstance::Init()
{
	particles.resize(emitter->maxParticles);

	particleIndices = new unsigned int[emitter->maxParticles];

	for (uint i = 0; i < emitter->maxParticles; ++i)
	{
		particleIndices[i] = i;
	}
}

bool EmitterInstance::Update(float dt)
{
	KillDeadParticles();

	//update modules
	for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
	{
		(*it)->Update(dt, this);
	}

	//add particle to render list
	//it is done at the component particle
	return true;
}

void EmitterInstance::DrawParticles()
{
	for (std::vector<Particle>::iterator it = particles.begin(); it < particles.end(); ++it)
	{
		it->Draw(0, 0);
	}
}

void EmitterInstance::SpawnParticle()
{
	if (deactivateParticleEmission || activeParticles == emitter->maxParticles)
	{
		return;
	}

	unsigned int particleIndex = particleIndices[activeParticles];
	Particle* particle = &particles[particleIndex];

	for (unsigned int i = 0; i < emitter->modules.size(); i++)
	{
		emitter->modules[i]->Spawn(particle, this);
	}

	++activeParticles;
}

void EmitterInstance::KillDeadParticles()
{
	//loop through every active particles to see if they are still active. In case one is inactive,
	//swap the new dead particle with the last particle alive and subtract 1 to activeParticles.
	for (int i = (activeParticles - 1); i >= 0; --i)
	{
		unsigned int particleIndex = particleIndices[i];
		Particle* particle = &particles[particleIndex];

		if (particle->lifeTime >= particle->maxLifetime)
		{
			if (i != (activeParticles - 1)) //if the last active particle is not active, skip the unnecesary changes
			{
				particleIndices[i] = particleIndices[activeParticles - 1];
				particleIndices[activeParticles - 1] = particleIndex;
			}

			--activeParticles;
		}
	}
}

void EmitterInstance::KillParticles()
{
	for (auto it : particles)
	{
		it.active = false;
	}
}

void EmitterInstance::SetParticleEmission(bool set)
{
	deactivateParticleEmission = !set;
}