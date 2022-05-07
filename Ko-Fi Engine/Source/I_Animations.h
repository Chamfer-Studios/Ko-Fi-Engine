#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include "Assimp.h"

class R_Animation;
class KoFiEngine;
struct Channel;

typedef unsigned int uint;

class I_Animations
{
public:
	I_Animations(KoFiEngine* engine);
	~I_Animations();

	bool Import(const aiAnimation* aiAnim, R_Animation* anim);
	bool Save(const R_Animation* animation, const char* path);
	bool Load(const char* path, R_Animation* animation);

	// Utilities for this importer
	void GetPositionKeys(const aiNodeAnim* aiChannel, Channel& rChannel);
	void GetRotationKeys(const aiNodeAnim* aiChannel, Channel& rChannel);
	void GetScaleKeys(const aiNodeAnim* aiChannel, Channel& rChannel);

	uint GetChannelsDataSize(const R_Animation* rAnimation);

	void ValidateChannel(Channel& rChannel); // Detects and erases any discrepancies with the given channel.
	void FuseChannels(const Channel& newChannel, Channel& existingChannel); // Fuses channels with the same name. New are fused into existing.

private:
	KoFiEngine* engine = nullptr;
};
#endif // !__I_ANIMATIONS_H__
