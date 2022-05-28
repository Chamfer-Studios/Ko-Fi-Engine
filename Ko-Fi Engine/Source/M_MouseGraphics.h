#ifndef __MOUSEGRAPHICS_H__
#define __MOUSEGRAPHICS_H__
#include "Module.h"
#include <string>
class R_Texture;
class M_MouseGraphics : public Module
{
public:
	M_MouseGraphics(KoFiEngine* engine);
	//destructor
	virtual ~M_MouseGraphics();

	bool Start();
	bool CleanUp();
	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------
		// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------
	// Method to receive and manage events
	void OnNotify(const Event& event);

	//Mouse Graphics functions
	void SetMouseTexture(std::string texturePath);
	void RenderMouseTexture();
private:
	KoFiEngine* engine = nullptr;
	R_Texture* currentMouseTexture = nullptr;
};

#endif //__MOUSEGRAPHICS_H__
