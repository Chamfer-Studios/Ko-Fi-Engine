#include "R_Texture.h"

R_Texture::R_Texture() : Resource(ResourceType::TEXTURE)
{
	textureID = TEXTUREID_DEFAULT;
	width = 0;
	height = 0;
	nrChannels = 1;
}

R_Texture::~R_Texture()
{
	path.clear();
	path.shrink_to_fit();
}