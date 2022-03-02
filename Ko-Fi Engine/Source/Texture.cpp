#include "Texture.h"

Texture::Texture()
{
	textureID = -1;
	width = -1;
	height = -1;
	nrChannels = 1;
}

Texture::~Texture()
{
	path.clear();
	path.shrink_to_fit();
}