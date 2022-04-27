#ifndef __I_TEXTURE_H__
#define __I_TEXTURE_H__
#include <string>

class R_Texture;
class KoFiEngine;

class I_Texture
{
public:
	I_Texture(KoFiEngine* engine);
	~I_Texture();

	bool Import(const char* path, R_Texture* texture);
	bool Save(const R_Texture* texture, const char* path);
	bool Load(const char* path, R_Texture* texture);

private:
	KoFiEngine* engine = nullptr;
};

#endif // !__I_TEXTURE_H__