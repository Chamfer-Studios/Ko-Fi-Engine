#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "MathGeoLib/Math/float3.h"
#include "C_Transform.h"
#include "Color.h"
#include "Globals.h"

class Particle
{
public:
	Particle();
	~Particle();

	bool Update(float dt);
	void Draw(uint id, uint indexNum);

public:
	bool active = false;

	//float4x4 transformMatrix;
	//float4x4 transformMatrixLocal;
	float3 position;
	Quat rotation;
	float initialRotation;
	float finalRotation;
	//float3 rotationEuler;
	float3 scale;
	float3 initialScale;
	float3 finalScale;

	float3 velocity;
	float3 acceleration;
	float3 direction;
	//float intensity = 1.0f;

	float lifeTime = 0.0f;
	float maxLifetime = 0.0f;
	float distanceToCamera = 0.0f;

	Color CurrentColor;
};

#endif // !__PARTICLE_H__