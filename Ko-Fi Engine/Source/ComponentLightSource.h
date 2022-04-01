#pragma once
#include "Component.h"
#include "GameObject.h"

#include "MathGeoLib/Math/float3.h"

enum class SourceType
{
	DIRECTIONAL,
	POINT,
	FOCAL
};

class LightSource
{
public:
	LightSource() 
	{
		position = float3::zero;
		color = float3(1.0f, 1.0f, 1.0f);

		ambient = 0.2;
		diffuse = 0.8;
		specular = 0;
	}

	~LightSource() {}

	float3 position;
	float3 color;

	float ambient;
	float diffuse;
	float specular;
};

//similar to sunlight, all rays are have the same direction and there is no perceivable attenuation
class DirectionalLight : public LightSource
{
public:
	DirectionalLight() { direction = float3(0.0f, 0.0f, 0.0f); }
	float3 GetDirection() { return direction; }
	void SetDirection(float3 direction) { this->direction = direction; }

	float3 direction;
};

//omnidirectional rays with attenuation following the next formula: 
//F = 1.0/(constantTerm, linearTerm * distance, quadraticTerm * distance^2)
class PointLight : public LightSource
{
public:
	PointLight() 
	{
		constant = 0.0f;
		linear = 0.0f;
		quadratic = 0.0f;
	}
	~PointLight() {}

	float constant;
	float linear;
	float quadratic;
};

//same as point light but acting only in a defined area. (can have blurred borders or not)
struct FocalLight : public LightSource
{

};

class ComponentLightSource : public Component
{
public:
	ComponentLightSource(GameObject* parent);
	~ComponentLightSource();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void Save(Json& json) const;
	void Load(Json& json);

	bool InspectorDraw(PanelChooser* chooser);

	//basic getters/setters
	LightSource* GetLightSource() const { return lightSource; }
	SourceType GetSourceType() const { return sourceType; }
	float3 GetColor() const { return lightSource->color; }
	float3 GetPosition() const { return lightSource->position; }
	//get Ambient, Diffuse and Specular parameters from 0 to 1 in a float3
	float3 GetADS() const { return float3(lightSource->ambient, lightSource->diffuse, lightSource->specular); }

	LightSource* ChangeSourceType(SourceType type);
	void SetColor(float3 color) { this->lightSource->color = color; }
	void SetPosition(float3 position) { this->lightSource->position = position; }
	//set Ambient, Diffuse and Specular parameters from 0 to 1 in a float3
	void SetADS(float3 ADS) { lightSource->ambient = ADS.x, lightSource->diffuse = ADS.y, lightSource->specular = ADS.z; }

private:

	//light parameters
	SourceType sourceType;

	//updated number of each light type
	int numOfDirectional = 0;
	int numOfPoint = 0;
	int numOfFocal = 0;

	LightSource* lightSource;

	//inspector variables
	int sType;
};
