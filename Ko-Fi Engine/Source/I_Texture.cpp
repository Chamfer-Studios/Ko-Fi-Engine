#include "I_Texture.h"
#include "Texture.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include "stb_image.h"

#define CHECKERS_SIZE 32

I_Texture::I_Texture()
{}

I_Texture::~I_Texture()
{}

bool I_Texture::Import(const char* path, Texture* texture)
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

		if (texture->textureID == 0)
			glGenTextures(1, &texture->textureID);

		glBindTexture(GL_TEXTURE_2D, texture->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_SIZE, CHECKERS_SIZE,
			0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
		return;
	}

	unsigned char* pixels = stbi_load(path, &texture->width, &texture->height, &texture->nrChannels, STBI_rgb);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (texture->textureID == 0)
		glGenTextures(1, &texture->textureID);

	glBindTexture(GL_TEXTURE_2D, texture->textureID);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		appLog->AddLog("%s", "Texture Image not loaded correctly");

	stbi_image_free(pixels);

	return true;
}

bool I_Texture::Save(const Texture* texture, const char* path)
{
	return true;
}