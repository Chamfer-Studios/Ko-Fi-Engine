#include "ParticleModule.h"

// Modules
#include "Engine.h"
#include "M_Camera3D.h"

// GameObject
#include "GameObject.h"
#include "C_Camera.h"

#include <vector>

#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "MathGeoLib/Math/MathFunc.h"
#include "MathGeoLib/Math/float3x3.h"

ParticleModule::ParticleModule()
{
	//if (emitter != nullptr)
	//{
	//	emitter->AddModule(this);
	//}
}

ParticleModule::~ParticleModule()
{}

void ParticleModule::Spawn(Particle* particle, EmitterInstance* emitter)
{}

bool ParticleModule::Update(float dt, EmitterInstance* emitter)
{
	return true;
}

float ParticleModule::GetPercentage(Particle* p)
{
	return (p->lifeTime / p->maxLifetime);
}

// MODULES---------------------------------------------------------------

EmitterDefault::EmitterDefault()
{
	type = ParticleModuleType::DEFAULT;
}

EmitterDefault::~EmitterDefault()
{
	instance = nullptr;
}

void EmitterDefault::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	GameObject* go = emitter->component->owner;
	instance = emitter;

	particle->position = go->GetTransform()->GetGlobalTransform().TranslatePart();

	if (randomParticleLife)
		particle->maxLifetime = math::Lerp(minParticleLife, maxParticleLife, random.Float());
	else
		particle->maxLifetime = minParticleLife;

	particle->lifeTime = 0.0f;
}

bool EmitterDefault::Update(float dt, EmitterInstance* instance)
{
	if (disable)
		return true;

	spawnTimer += dt;
	if (spawnTimer >= spawnTime)
	{
		spawnTimer = 0.0f;
		if (looping)
		{
			//in case the particlesPerBurst var is higher than the max particles of the emitter
			int burstCap = instance->emitter->maxParticles - instance->activeParticles;
			if (particlesPerSpawn > burstCap)
				instance->SpawnParticle(burstCap);
			else
				instance->SpawnParticle(particlesPerSpawn);
		}
	}

	for (unsigned int i = 0; i < instance->activeParticles; i++)
	{
		unsigned int particleIndex = instance->particleIndices[i];
		Particle* particle = &instance->particles[particleIndex];

		if (particle->active)
		{
			if (!randomParticleLife)
				particle->maxLifetime = minParticleLife;
			else
			{
				if (particle->maxLifetime >= maxParticleLife)
					particle->maxLifetime = maxParticleLife;
			}
			particle->lifeTime += dt;
			if (particle->lifeTime >= particle->maxLifetime)
				particle->lifeTime = particle->maxLifetime;
			particle->distanceToCamera = float3(instance->component->owner->GetEngine()->GetCamera3D()->currentCamera->
				GetCameraFrustum().WorldMatrix().TranslatePart() - particle->position).LengthSq();
		}
	}

	return true;
}

EmitterMovement::EmitterMovement()
{
	type = ParticleModuleType::MOVEMENT;
}

void EmitterMovement::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	if (randomPosition)
	{
		float positionX = math::Lerp(minPosition.x, maxPosition.x, random.Float());
		float positionY = math::Lerp(minPosition.y, maxPosition.y, random.Float());
		float positionZ = math::Lerp(minPosition.z, maxPosition.z, random.Float());
		particle->position += float3(positionX, positionY, positionZ);
	}
	else
		particle->position += minPosition;

	if (followForward)
	{
		particle->direction = -emitter->component->owner->GetTransform()->Front();
	}
	else
	{
		if (randomDirection)
		{
			float directionX = math::Lerp(minDirection.x, maxDirection.x, random.Float());
			float directionY = math::Lerp(minDirection.y, maxDirection.y, random.Float());
			float directionZ = math::Lerp(minDirection.z, maxDirection.z, random.Float());
			particle->direction = float3(directionX, directionY, directionZ);
		}
		else
			particle->direction = minDirection;
	}

	if (randomVelocity)
	{
		float intensity = math::Lerp(minVelocity, maxVelocity, random.Float());
		particle->velocity = particle->direction * intensity;
	}
	else
		particle->velocity = particle->direction * minVelocity;

	if (randomAcceleration)
	{
		float accelerationX = math::Lerp(minAcceleration.x, maxAcceleration.x, random.Float());
		float accelerationY = math::Lerp(minAcceleration.y, maxAcceleration.y, random.Float());
		float accelerationZ = math::Lerp(minAcceleration.z, maxAcceleration.z, random.Float());
		particle->acceleration = float3(accelerationX, accelerationY, accelerationZ);
	}
	else
		particle->acceleration = minAcceleration;
}

bool EmitterMovement::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
		return true;

	for (unsigned int i = 0; i < emitter->activeParticles; i++)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		if (particle->active)
		{
			particle->velocity += particle->acceleration;
			particle->position += particle->velocity * dt;
		}
	}
}

EmitterColor::EmitterColor()
{
	type = ParticleModuleType::COLOR;
	colorOverTime.push_back(FadeColor());
}

void EmitterColor::Spawn(Particle* particle, EmitterInstance* emitter)
{
	particle->CurrentColor = ColorLerp(GetPercentage(particle));
}

bool EmitterColor::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
		return true;

	for (unsigned int i = 0; i < emitter->activeParticles; i++)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		if(particle->active)
			particle->CurrentColor = ColorLerp(GetPercentage(particle));
	}
}

Color EmitterColor::ColorLerp(float current)
{
	for (int i = 0; i < colorOverTime.size(); ++i)
	{
		if (colorOverTime.at(i).pos == current)
		{
			return colorOverTime.at(i).color;
		}
		else if (i == 0 && colorOverTime.at(i).pos > current)
		{
			return colorOverTime.at(i).color;
		}
		else if (i == (colorOverTime.size()-1) && colorOverTime.at(i).pos < current)
		{
			return colorOverTime.at(i).color;
		}
		else if (colorOverTime.at(i).pos > current && colorOverTime.at(i-1).pos < current)
		{
			float r = math::Lerp(colorOverTime.at(i - 1).color.r, colorOverTime.at(i).color.r, current);
			float g = math::Lerp(colorOverTime.at(i - 1).color.g, colorOverTime.at(i).color.g, current);
			float b = math::Lerp(colorOverTime.at(i - 1).color.b, colorOverTime.at(i).color.b, current);
			float a = math::Lerp(colorOverTime.at(i - 1).color.a, colorOverTime.at(i).color.a, current);
			return Color(r, g, b, a);
		}
	}
	return Color(1.0f,1.0f,1.0f,1.0f);
}

//bool EmitterColor::EditColor(FadeColor& color, uint pos)
//{
//	bool ret = true;
//	ImVec4 vecColor = ImVec4(color.color.r, color.color.g, color.color.b, color.color.a);
//	//if (ImGui::ColorButton("Color", vecColor, ImGuiColorEditFlags_None, ImVec2(100, 20)));
//
//
//	//ImGui::SameLine();
//	//ImGui::TextUnformatted("Color");
//	if (pos > 0)
//	{
//		std::string colorStr = "Remove Color ";
//		colorStr.append(std::to_string(pos));
//		if (ImGui::Button(colorStr.c_str(), ImVec2(125, 25))) ret = false;
//	}
//
//	ImGui::ColorEdit4("Color", &color.color.a, ImGuiColorEditFlags_AlphaBar);
//
//	return ret;
//}

EmitterSize::EmitterSize()
{
	type = ParticleModuleType::SIZE;
}

void EmitterSize::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	if (constantSize)
		particle->scale = particle->initialScale = particle->finalScale = minInitialSize;
	else
	{
		if (randomInitialSize)
			particle->scale = particle->initialScale = particle->scale.Lerp(minInitialSize, maxInitialSize, random.Float());
		else
			particle->scale = particle->initialScale = minInitialSize;

		if (randomFinalSize)
			particle->finalScale = particle->scale.Lerp(minFinalSize, maxFinalSize, random.Float());
		else
			particle->finalScale = minFinalSize;

	}
}

bool EmitterSize::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
		return true;

	if (!constantSize)
	{
		for (unsigned int i = 0; i < emitter->activeParticles; i++)
		{
			unsigned int particleIndex = emitter->particleIndices[i];
			Particle* particle = &emitter->particles[particleIndex];

			if (particle->active)
				particle->scale = particle->scale.Lerp(particle->initialScale, particle->finalScale, GetPercentage(particle));
		}
	}
}

ParticleBillboarding::ParticleBillboarding(BillboardingType typeB)
{
	type = ParticleModuleType::BILLBOARDING;
	billboardingType = typeB;
}

void ParticleBillboarding::Spawn(Particle* particle, EmitterInstance* emitter)
{
	LCG random;
	if (rangeDegrees)
		particle->degrees = (int)Lerp(minDegrees, maxDegrees, random.Float());
	else
		particle->degrees = minDegrees;
	particle->rotation = GetAlignmentRotation(particle->position, 0, emitter, emitter->component->owner->GetEngine()->GetCamera3D()->currentCamera->GetCameraFrustum().WorldMatrix());
}

bool ParticleBillboarding::Update(float dt, EmitterInstance* emitter)
{
	if (disable)
		return true;

	for (unsigned int i = 0; i < emitter->activeParticles; ++i)
	{
		unsigned int particleIndex = emitter->particleIndices[i];
		Particle* particle = &emitter->particles[particleIndex];

		if(particle->active)
			particle->rotation = GetAlignmentRotation(particle->position, particle->degrees, emitter, emitter->component->owner->GetEngine()->GetCamera3D()->currentCamera->GetCameraFrustum().WorldMatrix());
	}
}

Quat ParticleBillboarding::GetAlignmentRotation(const float3& position,const int degrees, EmitterInstance* emitter, const float4x4& cameraTransform)
{
	float3 N, U, _U, R;
	float3 direction = float3(cameraTransform.TranslatePart() - position).Normalized(); //normalized vector between the camera and gameobject position

	switch (billboardingType)
	{
	case(BillboardingType::SCREEN_ALIGNED):
	{
		N = cameraTransform.WorldZ().Normalized().Neg();	// N is the inverse of the camera +Z
		U = cameraTransform.WorldY().Normalized();			// U is the up vector from the camera (already perpendicular to N)
		R = U.Cross(N).Normalized();						// R is the cross product between  U and N
	}
	break;
	case(BillboardingType::WORLD_ALIGNED):
	{
		N = direction;										// N is the direction
		_U = cameraTransform.WorldY().Normalized();			// _U is the up vector form the camera, only used to calculate R
		R = _U.Cross(N).Normalized();						// R is the cross product between U and N
		U = N.Cross(R).Normalized();						// U is the cross product between N and R
	}
	break;
	case(BillboardingType::X_AXIS_ALIGNED):
	{
		R = float3::unitX;									// R = (1,0,0)
		U = direction.Cross(R).Normalized();				// U cross between R and direction
		N = R.Cross(U).Normalized();						// N faces the camera
	}
	break;
	case(BillboardingType::Y_AXIS_ALIGNED):
	{
		U = float3::unitY;
		R = U.Cross(direction).Normalized();
		N = R.Cross(U).Normalized();
	}
	break;
	case(BillboardingType::Z_AXIS_ALIGNED):
	{
		N = float3::unitZ;
		R = direction.Cross(N).Normalized();
		U = N.Cross(R).Normalized();
	}
	break;
	case(BillboardingType::XZ_AXIS_LOCKED):
	{						
		N = float3::unitY;	
		U = float3::unitZ.Neg();
		R = float3::unitX;
	}
	}

	float3x3 result = float3x3(R, U, N);
	
	if (billboardingType == BillboardingType::XZ_AXIS_LOCKED)
		result = emitter->component->owner->GetTransform()->GetLocalTransform().RotatePart() * result;

	Quat rot;
	if (frontAxis)
		result = rot.RotateZ(DegToRad(degrees)).Normalized() * result;
	if (topAxis)
		result = rot.RotateY(DegToRad(degrees)).Normalized() * result;
	if (sideAxis)
		result = rot.RotateX(DegToRad(degrees)).Normalized() * result;

	return result.ToQuat();
}

const char* ParticleBillboarding::BillboardTypeToString(ParticleBillboarding::BillboardingType e)
{
	const std::map<ParticleBillboarding::BillboardingType, const char*> moduleTypeStrings{
		{ParticleBillboarding::BillboardingType::SCREEN_ALIGNED, "Screen Aligned"},
		{ParticleBillboarding::BillboardingType::WORLD_ALIGNED, "World Aligned"},
		{ParticleBillboarding::BillboardingType::X_AXIS_ALIGNED, "X Axis Aligned"},
		{ParticleBillboarding::BillboardingType::Y_AXIS_ALIGNED, "Y Axis Aligned"},
		{ParticleBillboarding::BillboardingType::Z_AXIS_ALIGNED, "Z Axis Aligned"},
		{ParticleBillboarding::BillboardingType::XZ_AXIS_LOCKED, "xy Axis locked"},
	};
	auto   it = moduleTypeStrings.find(e);
	return it == moduleTypeStrings.end() ? "Out of range" : it->second;
}
