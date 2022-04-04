#include "Texture.h"

Texture::Texture()
{
	textureID = TEXTUREID_DEFAULT;
	width = 0;
	height = 0;
	nrChannels = 1;
}

Texture::~Texture()
{
	CleanUp();
}

void Texture::CleanUp()
{
	path.clear();
	path.shrink_to_fit();
}
