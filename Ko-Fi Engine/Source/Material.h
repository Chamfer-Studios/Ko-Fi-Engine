#pragma once
#include "MathGeoLib/Math/float4.h"
#include "Texture.h"
#include <string>
class Material {
public:
	Material() {}
	~Material() {}
public:
	std::string materialName;
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