#ifndef __C_AUDIO_SWITCH_H__
#define __C_AUDIO_SWITCH_H__

#include "C_Audio.h"

class GameObject;

class C_AudioSwitch : public C_Audio
{
public:
    C_AudioSwitch(GameObject* parent) : C_Audio(parent) {}
    ~C_AudioSwitch() {}
};

#endif // !__C_AUDIO_SWITCH_H__