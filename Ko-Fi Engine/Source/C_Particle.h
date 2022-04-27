#ifndef __C_PARTICLE_H__
#define __C_PARTICLE_H__

#include "Component.h"
#include "EmitterInstance.h"
#include "SceneIntro.h"
#include "Color.h"

class EmitterInstance;
class R_ParticleResource;
class GameObject;

class C_Particle : public Component
{
public:
	C_Particle(GameObject* parent);
	~C_Particle();

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
	R_ParticleResource* resource = nullptr;
	int moduleToAdd = 0;
	int resourceToAdd = 0;
	int currentTextureId = 0;
	int billboardingType = 0;
	
};

#endif // __C_PARTICLE_H__