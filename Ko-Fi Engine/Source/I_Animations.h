#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include "Assimp.h"

class Animation;

class I_Animations
{
public:
	I_Animations();
	~I_Animations();

	bool Import(const aiAnimation* aiAnim, Animation* anim, const aiScene* assimpScene = nullptr);
	
};
#endif // !__I_ANIMATIONS_H__
