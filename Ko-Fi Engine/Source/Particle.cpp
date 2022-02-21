#include "Particle.h"

Particle::Particle()
{
	active = true;
	position = float3::zero;
	scale = float3(1.0f, 1.0f, 1.0f);
	velocity = float3::zero;
	acceleration = float3::zero;
	direction = float3::zero;
	intensity = 1.0f;
	lifeTime = 0.0f;
	maxLifetime = 0.0f;
	distanceToCamera = 0.0f;
	CurrentColor = Color();
}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	if (!active)
	{
		return true;
	}

	return true;
}