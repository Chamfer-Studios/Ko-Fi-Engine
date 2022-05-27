#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "Engine.h"
#include "M_UI.h"
#include "SDL_image.h"
#include "SDL_mouse.h"

class Cursor {
public: 
	void Draw() {
		SDL_RenderCopy(engine->GetUI()->renderer, cursorTex, NULL, &rect);
	}

private:
	KoFiEngine* engine = nullptr;

	SDL_Texture* cursorTex;
	SDL_Rect rect;
	SDL_Rect point;
};
#endif
