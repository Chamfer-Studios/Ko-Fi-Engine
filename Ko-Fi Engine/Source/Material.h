#pragma once
#include "MathGeoLib/Math/float4.h"
#include "Texture.h"
class Material {
public:
	Material() {}
	~Material() {}
public:
	//albedo
	float4 albedoTint;
	Texture textureAlbedo;
	//metallic
	float metallicValue;
	//roughness
	float roughnessValue;
	//ambient occlusion
	float ambientOcclusionValue;
	
};