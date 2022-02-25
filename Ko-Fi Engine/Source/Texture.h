#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>
#include "MathGeoLib/Math/float2.h"
#include "Globals.h"
#include "Resource.h"

//class Texture
//{
//public:
//	Texture(){}
//	void SetUpTexture(std::string path = "");
//	//Getters
//	uint GetTextureId() { return textureID; }
//	int GetTextureWidth() { return width; }
//	int GetTextureHeight() { return height; }
//	int GetNrChannels() { return nrChannels; }
//	const char* GetTexturePath() { return texturePath.c_str(); }
//	bool operator==(const Texture& tex) {
//		return tex.textureID == this->textureID;
//	}
//private:
//	uint textureID = 0;
//	int width, height, nrChannels;
//	std::string texturePath;
//	float2 scale;
//	float2 offset;
//};

class Texture : public Resource
{
public:
	Texture();
	~Texture();

	void SetUpTexture();

public:
	uint textureID = -1;
	int width = -1;
	int height = -1;
	int nrChannels = 1;
};

#endif // !__TEXTURE_H__