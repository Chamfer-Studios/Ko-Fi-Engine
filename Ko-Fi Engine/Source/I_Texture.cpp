#include "I_Texture.h"
#include "R_Texture.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include "stb_image.h"
#include <string>

#define CHECKERS_SIZE 32

I_Texture::I_Texture()
{}

I_Texture::~I_Texture()
{}

bool I_Texture::Import(const char* path, R_Texture* texture)
{
	texture->SetTexturePath(path);

	if (path == nullptr)
	{
		GLubyte checkerImage[CHECKERS_SIZE][CHECKERS_SIZE][4];
		for (int i = 0; i < CHECKERS_SIZE; i++)
		{
			for (int j = 0; j < CHECKERS_SIZE; j++)
			{
				int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
				checkerImage[i][j][0] = (GLubyte)c;
				checkerImage[i][j][1] = (GLubyte)c;
				checkerImage[i][j][2] = (GLubyte)c;
				checkerImage[i][j][3] = (GLubyte)255;
			}
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (texture->GetTextureId() == TEXTUREID_DEFAULT)
		{
			uint id = texture->GetTextureId();
			glGenTextures(1, &id);
			texture->SetTextureId(id);
		}

		glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_SIZE, CHECKERS_SIZE,
			0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

		return true;
	}
	int width = texture->GetTextureWidth();
	int height = texture->GetTextureHeight();
	int channels = texture->GetNrChannels();
	unsigned char* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	texture->SetTextureWidth(width);
	texture->SetTextureHeight(height);
	texture->SetNrChannels(channels);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (texture->GetTextureId() == TEXTUREID_DEFAULT)
	{
		uint id = texture->GetTextureId();
		glGenTextures(1, &id);
		texture->SetTextureId(id);
	}

	glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->GetTextureWidth(), texture->GetTextureHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		appLog->AddLog("%s", "Texture Image not loaded correctly: %s\n", path);

	stbi_image_free(pixels);

	return true;
}

bool I_Texture::Save(const R_Texture* texture, const char* path)
{

}