#include "I_Texture.h"
#include "Texture.h"

I_Texture::I_Texture()
{}

I_Texture::~I_Texture()
{}

bool I_Texture::Import(const char* path, Texture* texture)
{
	return true;
}

bool I_Texture::Save(const Texture* texture, const char* path)
{
	return true;
}

bool I_Texture::Load(const char* path, Texture* texture)
{
	return true;
}