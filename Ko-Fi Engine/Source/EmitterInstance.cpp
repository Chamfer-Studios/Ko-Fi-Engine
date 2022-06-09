#include "EmitterInstance.h"
#include "ParticleModule.h"
#include "R_Texture.h"
#include "M_ResourceManager.h"
#include "Engine.h"
#include "Log.h"

EmitterInstance::EmitterInstance(Emitter* e, C_Particle* cp) : emitter(e),component(cp)
{
	emitterLife = 0.0f;
	activeParticles = 0;
	particles.clear();
	particles.shrink_to_fit();
}

EmitterInstance::~EmitterInstance()
{
	component->owner->GetEngine()->GetResourceManager()->FreeResource(emitter->texture->GetUID());


	emitter = nullptr;



	particles.clear();
	particles.shrink_to_fit();

	component = nullptr;


	RELEASE(particleIndices);
}

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

void EmitterInstance::SpawnParticle(int burst)
{
	if (deactivateParticleEmission || activeParticles == emitter->maxParticles)
		return;

	//burst to generate more than particle at once
	for (int j = 0; j < burst; j++)
	{
		unsigned int particleIndex = particleIndices[activeParticles];
		Particle* particle = &particles[particleIndex];
		if (particle->active)
			CONSOLE_LOG("EHHHHHH!");

		particle->active = true;

		for (unsigned int i = 0; i < emitter->modules.size(); i++)
		{
			emitter->modules[i]->Spawn(particle, this);
		}
	
		++activeParticles;
	}
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
			particle->lifeTime = particle->maxLifetime;
			particle->active = false;

			--activeParticles;
			if (!loop)
			{
				for (auto m : emitter->modules)
				{
					if (m->type == ParticleModuleType::DEFAULT)
					{
						EmitterDefault* mDef = (EmitterDefault*)m;
						mDef->looping = false;
					}
				}
			}
		}
	}
}

void EmitterInstance::KillAllParticles()
{
	for (auto it : particles)
		it.active = false;
	activeParticles = 0;
}

void EmitterInstance::SetParticleEmission(bool set)
{
	deactivateParticleEmission = !set;
}

bool EmitterInstance::GetParticleEmission()
{
	return !deactivateParticleEmission;
}
