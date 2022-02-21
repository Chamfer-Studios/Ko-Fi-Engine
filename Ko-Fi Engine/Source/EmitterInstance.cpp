#include "EmitterInstance.h"

EmitterInstance::EmitterInstance(Emitter* e, ComponentParticle* cp) : emitter(e),component(cp)
{
	emitterLife = 0.0f;
	activeParticles = 0;
	particles.clear();
}

EmitterInstance::~EmitterInstance()
{}

bool EmitterInstance::Update(float dt)
{
	for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it)
	{
		if (!(*it)->active)
		{
			it = particles.erase(it);
			--activeParticles;
			if (it == particles.begin())
			{
				--it;
			}
			else if (it == particles.end())
			{
				break;
			}
		}
		// MAYBE????????
		//else
		//{
		//	(*it)->Update(dt);
		//}
	}

	for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
	{
		(*it)->Update(dt, this);
	}

	// TODO: DRAW HERE THE IMAGE

	return true;
}

void EmitterInstance::SpawnParticle()
{
	if (deactivateParticleEmission || activeParticles == emitter->maxParticles)
	{
		return;
	}
	Particle* p = new Particle();
	particles.push_back(p);
	++activeParticles;
	for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
	{
		(*it)->Spawn(p, this);
	}
}

void EmitterInstance::KillParticles()
{
	for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it)
	{
		(*it)->active = false;
	}
}

void EmitterInstance::SetParticleEmission(bool set)
{
	deactivateParticleEmission = !set;
}