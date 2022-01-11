#pragma once
typedef unsigned int uint;
#include <string>

class Texture
{
public:
	Texture(){}
	void SetUpTexture(const char* path = nullptr);
	//Getters
	uint GetTextureId() { return textureID; }
	int GetTextureWidth() { return width; }
	int GetTextureHeight() { return height; }
	int GetNrChannels() { return nrChannels; }
	const char* GetTexturePath() { return texturePath.c_str(); }
private:
	uint textureID = 0;
	int width, height, nrChannels;
	std::string texturePath;
};