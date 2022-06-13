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

	bool Import(const char* path, R_Texture* texture, bool cleanAfterImport = false);
	bool Save(R_Texture* texture, const char* path);
	bool Load(const char* path, R_Texture* texture);

	R_Texture* GetCheckerTexture();

private:
	KoFiEngine* engine = nullptr;

	R_Texture* checkerTexture = nullptr;
};

#endif // !__I_TEXTURE_H__