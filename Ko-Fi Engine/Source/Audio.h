#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

#include "Module.h"
#include <iostream>

class Audio : public Module
{
public:

    Audio(KoFiEngine* engine);
    ~Audio();

    bool Awake(Json configModule);
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    void OnNotify(const Event& event);

    void CreateAudioListener(float x, float y, float z);

    // Engine config serialization --------------------------------------
    bool SaveConfiguration(Json& configModule) const override;
    bool LoadConfiguration(Json& configModule) override;
    // ------------------------------------------------------------------

    // Engine config inspector draw -------------------------------------
    bool InspectorDraw() override;
    // ------------------------------------------------------------------

private:
    void ListAudioDevices(const ALCchar* devices);

private:

    KoFiEngine* engine = nullptr;

    ALCdevice* device;
    ALCcontext* context;
};

#endif // !__AUDIO_H__