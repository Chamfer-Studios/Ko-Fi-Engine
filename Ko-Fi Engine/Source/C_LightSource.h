#ifndef __C_LIGHTSOURCE_H__
#define __C_LIGHTSOURCE_H__

#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"

class GameObject;
class C_Camera;

enum class SourceType
{
	DIRECTIONAL,
	POINT,
	FOCAL
};

class LightSource
{
public:
	LightSource();

	float3 position; //global
	float3 color;

	float ambient;
	float diffuse;
	float specular;
};

//similar to sunlight, all rays are have the same direction and there is no perceivable attenuation
class DirectionalLight : public LightSource
{
public:
	DirectionalLight();
	float3 GetDirection() { return direction; }
	void SetDirection(float3 direction) { this->direction = direction; }

	float3 direction;
	float4x4 lightSpaceMatrix;
};

//omnidirectional rays with attenuation following the next formula: 
//F = 1.0/(constantTerm, linearTerm * distance, quadraticTerm * distance^2)
class PointLight : public LightSource
{
public:
	PointLight();

	float constant;
	float linear;
	float quadratic;

};

//same as point light but acting only in a defined area. (can have blurred borders or not)
struct FocalLight : public LightSource
{
public:
	FocalLight();

	float cutOffAngle;
	float3 lightDirection;

	float constant;
	float linear;
	float quadratic;

};

class C_LightSource : public Component
{
public:
	C_LightSource(GameObject* parent);
	~C_LightSource();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
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
	
	void CastShadows();
private:

	//light parameters
	SourceType sourceType = SourceType::DIRECTIONAL;

	//updated number of each light type
	int numOfDirectional = 0;
	int numOfPoint = 0;
	int numOfFocal = 0;

	LightSource* lightSource;
	C_Camera* shadowCam;

	//inspector variables
	int sType = 0;
	bool castShadows = false;
	float hFov = 0.0f;
	float vFov = 0.0f;
};

#endif // !__C_LIGHTSOURCE_H__