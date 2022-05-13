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
		BILLBOARDING,
		END
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
	uint particlesPerSpawn = 1;
	EmitterInstance* instance = nullptr;
};

class EmitterMovement : public ParticleModule
{
public:
	EmitterMovement();

	void Spawn(Particle* particle, EmitterInstance* emitter);
	bool Update(float dt, EmitterInstance* emitter);

public:
	bool followForward = true;
	bool randomDirection = false;
	float3 minDirection = float3(1.0f, 1.0f, 1.0f);
	float3 maxDirection = float3(-1.0f, -1.0f, -1.0f);

	bool randomPosition = false;
	float3 minPosition = float3::zero;
	float3 maxPosition = float3::zero;

	bool randomVelocity = false;
	float minVelocity = 3.0f;
	float maxVelocity = 5.0f;

	bool randomAcceleration = false;
	float3 minAcceleration = float3(-0.25f, 0.0f, -0.25f);
	float3 maxAcceleration = float3(0.25f, 0.25f, 0.25f);
};

class EmitterColor : public ParticleModule
{
public:
	EmitterColor();

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
	bool constantSize = true;
	bool randomInitialSize = false;
	bool randomFinalSize = false;
	float3 minInitialSize = float3(1.0f, 1.0f, 1.0f);
	float3 maxInitialSize = float3(1.5f, 1.5f, 1.5f);
	float3 minFinalSize = float3(0.5f, 0.5f, 0.5f);
	float3 maxFinalSize = float3(0.75f, 0.75f, 0.75f);
};

class ParticleBillboarding : public ParticleModule
{
public:
	enum class BillboardingType
	{
		NONE,
		SCREEN_ALIGNED,
		WORLD_ALIGNED,
		X_AXIS_ALIGNED,
		Y_AXIS_ALIGNED,
		Z_AXIS_ALIGNED,
		END
	};

	ParticleBillboarding(BillboardingType typeB = BillboardingType::WORLD_ALIGNED);

	void Spawn(EmitterInstance* emitter, Particle* particle);
	bool Update(float dt, EmitterInstance* emitter);

	Quat GetAlignmentRotation(const float3& position, const float4x4& cameraTransform);
	const char* BillboardTypeToString(ParticleBillboarding::BillboardingType e);

	BillboardingType billboardingType = BillboardingType::WORLD_ALIGNED;
	bool hideBillboarding = false;
};

#endif // !__PARTICLE_MODULE_H__