#ifndef __C_AUDIO_SPACIAL_H__
#define __C_AUDIO_SPACIAL_H__

#include "C_Audio.h"

class GameObject;

class C_AudioSpacial : public C_Audio
{
public:
    C_AudioSpacial(GameObject* parent) : C_Audio(parent) {}
    virtual ~C_AudioSpacial() {}
};

#endif // !__C_AUDIO_SPACIAL_H__