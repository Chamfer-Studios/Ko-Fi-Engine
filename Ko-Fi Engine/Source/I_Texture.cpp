#include "I_Texture.h"
#include "R_Texture.h"
#include "Engine.h"
#include "M_FileSystem.h"
#include "FSDefs.h"
#include "Log.h"
#include "ImGuiAppLog.h"

#include "glew.h"
#include "stb_image.h"

#include <string>
#include <fstream>

#define CHECKERS_SIZE 32

I_Texture::I_Texture(KoFiEngine* engine) : engine(engine)
{}

I_Texture::~I_Texture()
{}

bool I_Texture::Import(const char* path, R_Texture* texture)
{
	if (texture == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Texture! Error: R_Texture* was nullptr.");
		return false;
	}

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

		texture->SetUpTexture(true);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_SIZE, CHECKERS_SIZE,
			0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

		return true;
	}
	else
	{
		texture->data = stbi_load(path, &texture->width, &texture->height, &texture->nrChannels, 0);

		texture->imageSizeBytes = texture->GetTextureWidth() * texture->GetTextureHeight() * texture->GetNrChannels() * sizeof(unsigned char);
		
		texture->SetUpTexture(false);
	}

	return true;
}

bool I_Texture::Save(const R_Texture* texture, const char* path)
{
	if (engine->GetFileSystem()->CheckDirectory(TEXTURES_DIR))
	{
		std::ofstream file;
		file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
		if (file.is_open())
		{
			// HEADER
			file.write((char*)&texture->width, sizeof(int));
			file.write((char*)&texture->height, sizeof(int));
			file.write((char*)&texture->nrChannels, sizeof(int));

			// BODY
			file.write((char*)texture->data, texture->imageSizeBytes);

			file.close();

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Texture Save: directory %s couldn't be accessed.", TEXTURES_DIR);

	return false;
}

bool I_Texture::Load(const char* path, R_Texture* texture)
{
	if (engine->GetFileSystem()->CheckDirectory(TEXTURES_DIR))
	{
		std::ifstream file;
		file.open(path, std::ios::binary);
		if (file.is_open())
		{
			// HEADER
			file.read((char*)&texture->width, sizeof(int));
			file.read((char*)&texture->height, sizeof(int));
			file.read((char*)&texture->nrChannels, sizeof(int));

			// BODY
			texture->imageSizeBytes = texture->width * texture->height * texture->nrChannels * sizeof(unsigned char);
			texture->data = (unsigned char*)malloc(texture->imageSizeBytes);
			file.read((char*)texture->data, texture->imageSizeBytes);

			file.close();

			texture->SetUpTexture(false);

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Texture Load: directory %s couldn't be accessed.", TEXTURES_DIR);

	return false;
}