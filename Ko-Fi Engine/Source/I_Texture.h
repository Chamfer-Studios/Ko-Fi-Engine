#ifndef __I_TEXTURE_H__
#define __I_TEXTURE_H__

class Texture;

class I_Texture
{
public:
	I_Texture();
	~I_Texture();

	bool Import(const char* path, Texture* texture);
};

#endif // !__I_TEXTURE_H__