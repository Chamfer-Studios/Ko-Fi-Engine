#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>
#include "MathGeoLib/Math/float2.h"
#include "Globals.h"
//#include "Resource.h"

class Texture
{
public:
	Texture();
	~Texture();

	inline const char* GetTexturePath() { return path.c_str(); }
	inline void SetTexturePath(const char* path) { path = path; }

	inline uint GetTextureId() { return textureID; }
	inline int GetTextureWidth() { return width; }
	inline int GetTextureHeight() { return height; }
	inline int GetNrChannels() { return nrChannels; }

	void SetUpTexture();

public:
	uint textureID = -1;
	int width = -1;
	int height = -1;
	int nrChannels = 1;

	std::string path;
};

#endif // !__TEXTURE_H__