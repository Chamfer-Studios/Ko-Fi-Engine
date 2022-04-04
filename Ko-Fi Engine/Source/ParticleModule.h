#ifndef __PARTICLE_MODULE_H__
#define __PARTICLE_MODULE_H__

#include "EmitterInstance.h"
#include "Particle.h"

class Particle;
class EmitterInstance;

	enum class ParticleModuleType
	{
		NONE = 0,
		DEFAULT,
		MOVEMENT,
		COLOR,
		SIZE,
		BILLBOARDING
	};

class ParticleModule
{
public:

	ParticleModule();
	~ParticleModule();
	virtual void Spawn(Particle* particle, EmitterInstance* emitter);
	virtual bool Update(float dt, EmitterInstance* emitter);
	float GetPercentage(Particle* p);

public:
	ParticleModuleType type = ParticleModuleType::NONE;
	//Emitter* emitter = nullptr;
	bool disable = false;
};

// MODULES----------------------------------------------------------

class EmitterDefault : public ParticleModule
{
public:
	EmitterDefault();

	void Spawn(Particle* particle, EmitterInstance* emitter);
	bool Update(float dt, EmitterInstance* emitter);

public:
	float spawnTimer = 0.0f;
	float spawnTime = 0.5f;
	bool randomParticleLife = false;
	float minParticleLife = 1.0f;
	float maxParticleLife = 3.0f;
};

class EmitterMovement : public ParticleModule
{
public:
	EmitterMovement();

	void Spawn(Particle* particle, EmitterInstance* emitter);
	bool Update(float dt, EmitterInstance* emitter);

public:
	bool randomDirection = false;
	float3 minDirection = float3(1.0f, 1.0f, 1.0f);
	float3 maxDirection = float3(-1.0f, -1.0f, -1.0f);

	bool randomPosition = false;
	float3 minPosition = float3::zero;
	float3 maxPosition = float3::zero;

	bool randomVelocity = false;
	float minVelocity = 1.0f;
	float maxVelocity = 3.0f;

	bool randomAcceleration = false;
	float3 minAcceleration = float3(-1.0f, 0.0f, -1.0f);
	float3 maxAcceleration = float3(1.0f, 1.0f, 1.0f);
};

class EmitterColor : public ParticleModule
{
public:
	EmitterColor();
	~EmitterColor();

	void Spawn(Particle* particle, EmitterInstance* emitter);
	bool Update(float dt, EmitterInstance* emitter);
	Color ColorLerp(float current);
	//bool EditColor(FadeColor& color, uint pos = 0);

public:
	std::vector<FadeColor> colorOverTime;
};

class EmitterSize : public ParticleModule
{
public:
	EmitterSize();

	void Spawn(Particle* particle, EmitterInstance* emitter);
	bool Update(float dt, EmitterInstance* emitter);

public:
	float3 minSize = float3(1.0f, 1.0f, 1.0f);
	float3 maxSize = float3(1.5f, 1.5f, 1.5f);
};

class ParticleBillboarding : public ParticleModule
{
public:
	enum class BillboardingType
	{
		ScreenAligned,
		WorldAligned,
		XAxisAligned,
		YAxisAligned,
		ZAxisAligned,

		None,
	};

	ParticleBillboarding(BillboardingType typeB = BillboardingType::WorldAligned);

	void Spawn(EmitterInstance* emitter, Particle* particle);
	bool Update(float dt, EmitterInstance* emitter);

	Quat GetAlignmentRotation(const float3& position, const float4x4& cameraTransform);

	BillboardingType billboardingType = BillboardingType::WorldAligned;
	bool hideBillboarding = false;
};

#endif // !__PARTICLE_MODULE_H__