#include <string>
#include "Color.h"

class Texture;

class Material
{
public:
	Material();
	~Material();

	inline void SetColor(Color color) { diffuseColor = color; }

public:
	Color diffuseColor;

	std::string materialName;
	std::string materialPath;
};