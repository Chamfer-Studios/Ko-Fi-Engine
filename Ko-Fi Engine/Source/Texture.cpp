#include "Texture.h"

Texture::Texture() : Resource(ResourceType::TEXTURE)
{
	textureID = TEXTUREID_DEFAULT;
	width = 0;
	height = 0;
	nrChannels = 1;
}

Texture::~Texture()
{
	path.clear();
	path.shrink_to_fit();
}