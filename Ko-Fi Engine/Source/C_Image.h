#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "Component.h"
#include "C_Transform2D.h"
#include "C_RenderedUI.h"

#include "R_Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string.h>
#include <vector>

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class SDL_Texture;
class SDL_Surface;
class MyPlane;

class C_Image : public C_RenderedUI {
public:
	C_Image(GameObject* parent);
	~C_Image();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void SetTexture(const char* path);

	void Draw() override;

	R_Texture openGLTexture;

private:
	void FreeTextures();
};

#endif

