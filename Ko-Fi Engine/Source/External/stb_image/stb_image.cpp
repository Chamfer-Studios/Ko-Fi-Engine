#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC(sz)			malloc(sz)
#define STBI_REALLOC(p,newsz)	realloc(p,newsz)
#define STBI_FREE(p)			free(p)  

#include "stb_image.h"
