#ifndef __COMPONENT_PARTICLE_H__
#define __COMPONENT_PARTICLE_H__

#include "Component.h"
#include "EmitterInstance.h"
#include "SceneIntro.h"
#include "Color.h"

class EmitterInstance;
class ParticleResource;

class ComponentParticle : public Component
{
public:
	ComponentParticle(GameObject* parent);
	~ComponentParticle();


	bool Start() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool InspectorDraw(PanelChooser* chooser) override;
	void InspectorDrawColor(std::string emitterName,FadeColor& color, int index);

	void ClearParticles();
	void StopParticleSpawn();
	void ResumeParticleSpawn();
private:
	void NewEmitterName(std::string& name, int n = 1);

public:
	std::vector<EmitterInstance*> emitterInstances;
	ParticleResource* resource = nullptr;
	int moduleToAdd = 0;
	int currentTextureId = 0;
};

#endif // __COMPONENT_PARTICLE_H__