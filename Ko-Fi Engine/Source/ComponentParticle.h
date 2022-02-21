#ifndef __COMPONENT_PARTICLE_H__
#define __COMPONENT_PARTICLE_H__

#include "Component.h"
#include "EmitterInstance.h"
#include "Emitter.h"
#include "SceneIntro.h"

class EmitterInstance;
class Emitter;
class TMPPlane;

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
	bool InspectorDraw(PanelChooser* chooser);
	void ClearParticles();
	void StopParticleSpawn();
	void ResumeParticleSpawn();

public:
	std::vector<EmitterInstance*> emitterInstances;
	std::vector<Emitter*> emitters;
	TMPPlane* plane = nullptr;
};

#endif // __COMPONENT_PARTICLE_H__