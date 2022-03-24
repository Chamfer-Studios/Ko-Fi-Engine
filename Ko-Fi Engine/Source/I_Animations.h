#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include "Assimp.h"

class R_Animation;
struct Channel;

class I_Animations
{
public:
	I_Animations();
	~I_Animations();

	bool Import(const aiAnimation* aiAnim, R_Animation* anim);

	// Utilities for this importer
	void GetPositionKeys(const aiNodeAnim* aiChannel, Channel& rChannel);
	void GetRotationKeys(const aiNodeAnim* aiChannel, Channel& rChannel);
	void GetScaleKeys(const aiNodeAnim* aiChannel, Channel& rChannel);

	void ValidateChannel(Channel& rChannel); // Detects and erases any discrepancies with the given channel.
	void FuseChannels(const Channel& newChannel, Channel& existingChannel); // Fuses channels with the same name. New are fused into existing.
};
#endif // !__I_ANIMATIONS_H__
