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

	inline uint GetTextureId() { return textureID; }
	inline int GetTextureWidth() { return width; }
	inline int GetTextureHeight() { return height; }
	inline int GetNrChannels() { return nrChannels; }
	inline const char* GetTexturePath() { return texturePath.c_str(); }

	inline void SetTexturePath(const char* path) { texturePath = path; }

	void SetUpTexture();

public:
	uint textureID = -1;
	int width = -1;
	int height = -1;
	int nrChannels = 1;

	std::string texturePath;
};

#endif // !__TEXTURE_H__