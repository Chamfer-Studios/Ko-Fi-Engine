#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "MathGeoLib/Math/float2.h"
#include "Globals.h"
#include "Resource.h"

#define TEXTUREID_DEFAULT 4294967295
class string;

class R_Texture : public Resource
{
public:
	R_Texture();
	~R_Texture();

	inline const char* GetTexturePath() const { return GetAssetPath(); }
	inline void SetTexturePath(const char* path)
	{ 
		path != nullptr ? SetAssetPath(path) : SetAssetPath("");
	}

	inline uint GetTextureId() const { return textureID; }
	inline void SetTextureId(const uint id) { textureID = id; }
	inline int GetTextureWidth() const { return width; }
	inline void SetTextureWidth(const int width) { this->width = width; }
	inline int GetTextureHeight() const { return height; }
	inline void SetTextureHeight(const int height) { this->height = height; }
	inline int GetNrChannels() const { return nrChannels; }
	inline void SetNrChannels(const int channels) { nrChannels = channels; }

private:
	uint textureID = TEXTUREID_DEFAULT;
	int width = -1;
	int height = -1;
	int bytePerPixel = 3; //rgb o 4 rgba
	int nrChannels = 1;

	// Total bytes to save: header + 256x128xbytes_per_pixel
};

#endif // !__TEXTURE_H__