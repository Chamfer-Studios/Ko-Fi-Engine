#include "ComponentParticle.h"

ComponentParticle::ComponentParticle(GameObject* parent) : Component(parent)
{
	emitterInstances.clear();
	emitters.clear();
	Emitter* e = new Emitter();
	emitters.push_back(e);
	EmitterInstance* eI = new EmitterInstance(e, this);
	emitterInstances.push_back(eI);
}

ComponentParticle::~ComponentParticle()
{
}

bool ComponentParticle::Start()
{
	return true;
}

bool ComponentParticle::Update(float dt)
{
	// SHOULD I DO THIS?????
	//for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	//{
	//	(*it)->Update(dt);
	//}

	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->Update(dt);
	}

	return true;
}

bool ComponentParticle::PostUpdate(float dt)
{
	return true;
}

bool ComponentParticle::CleanUp()
{
	//DELETE TEXTURES

	for (std::vector<Emitter*>::iterator it = emitters.begin(); it < emitters.end(); ++it)
	{
		it = emitters.erase(it);
	}

	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		it = emitterInstances.erase(it);
	}

	return true;
}

void ComponentParticle::ClearParticles()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->KillParticles();
	}
}

void ComponentParticle::StopParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(false);
	}
}

void ComponentParticle::ResumeParticleSpawn()
{
	for (std::vector<EmitterInstance*>::iterator it = emitterInstances.begin(); it < emitterInstances.end(); ++it)
	{
		(*it)->SetParticleEmission(true);
	}
}