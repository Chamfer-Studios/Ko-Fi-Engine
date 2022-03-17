#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "glmath.h"
#include "ComponentCamera.h"

class GameObject;

class Camera3D : public Module
{
public:
	Camera3D(KoFiEngine* engine);
	~Camera3D();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);

	void OnGui() override;
	//void OnSave(JSONWriter& writer) const override;
	//void OnLoad(const JSONReader& reader) override;
	GameObject* MousePicking();

public:
	ComponentCamera* camera = nullptr;
	ComponentCamera* engineCamera = nullptr;


private:

	KoFiEngine* engine = nullptr;
};

#endif // !__CAMERA_H__