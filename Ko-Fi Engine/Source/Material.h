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
	std::string materialPath;
	//Crear un vector de uniforms con id, type y valor y guardarlos ahi para luego sacar los datos
	std::vector<Texture> textures;
};