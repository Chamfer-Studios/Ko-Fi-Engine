#include "EmitterInstance.h"
#include "ParticleModule.h"
#include "Texture.h"

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
	}

	for (std::vector<ParticleModule*>::iterator it = emitter->modules.begin(); it < emitter->modules.end(); ++it)
	{
		(*it)->Update(dt, this);
	}

	// TODO: DRAW HERE THE IMAGE
	/*if (emitter->texture != nullptr)
	{
		for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it)
		{
			(*it)->Draw(emitter->texture->GetTextureId(), 0);
		}
	}
	else
	{
		for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it)
		{
			(*it)->Draw(0, 0);
		}
	}*/
	return true;
}

void EmitterInstance::DrawParticles()
{
	for (std::vector<Particle*>::iterator it = particles.begin(); it < particles.end(); ++it)
	{
		(*it)->Draw(0, 0);
	}
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
	for (auto it : particles)
	{
		it->active = false;
	}
}

void EmitterInstance::SetParticleEmission(bool set)
{
	deactivateParticleEmission = !set;
}