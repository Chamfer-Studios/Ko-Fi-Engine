#include "R_Texture.h"

#include "glew.h"
#include "stb_image.h"

R_Texture::R_Texture() : Resource(ResourceType::TEXTURE)
{
	data = nullptr;
	imageSizeBytes = 0;
	textureID = TEXTUREID_DEFAULT;
	width = -1;
	height = -1;
	nrChannels = 1;
}

R_Texture::~R_Texture()
{
	if (data != nullptr)
	{
		stbi_image_free(data);
		data = nullptr;
	}
	glDeleteTextures(1, (GLuint*)&textureID);
}

bool R_Texture::SaveMeta(Json& json) const
{
	json["contained_resources"].array();
	return true;
}

void R_Texture::SetUpTexture()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (textureID == TEXTUREID_DEFAULT)
		glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != nullptr)
	{
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		appLog->AddLog("Texture Image not loaded correctly.\n");

	glBindTexture(GL_TEXTURE_2D, 0);
}