#ifndef __I_TEXTURE_H__
#define __I_TEXTURE_H__

class Texture;

class I_Texture
{
	I_Texture();
	~I_Texture();

	bool Import(const aiMesh* aiMaterial, Texture* texture);
	bool Save(const Texture* texture, const char* path);
	bool Load(const char* path, Texture* texture);
};

#endif // !__I_TEXTURE_H__