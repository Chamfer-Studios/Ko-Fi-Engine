#ifndef __COMPONENT_PARTICLE_H__
#define __COMPONENT_PARTICLE_H__

#include "Component.h"
#include "EmitterInstance.h"
#include "Emitter.h"

class EmitterInstance;
class Emitter;

class ComponentParticle : public Component
{
public:
	ComponentParticle(GameObject* parent);
	~ComponentParticle();


	bool Start() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	// TODO: Save & Load
	void ClearParticles();
	void StopParticleSpawn();
	void ResumeParticleSpawn();

public:
	std::vector<EmitterInstance*> emitterInstances;
	std::vector<Emitter*> emitters;
};

#endif // __COMPONENT_PARTICLE_H__