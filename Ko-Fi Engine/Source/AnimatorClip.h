#ifndef __ANIMATOR_CLIP_H__
#define __ANIMATOR_CLIP_H__

#include <string>
typedef unsigned int uint;
class Animation;

class AnimatorClip
{
public:
	AnimatorClip();
	AnimatorClip(const Animation* animation, const std::string& name, uint start, uint end, float speed, bool loop);

	const std::string GetName() const;
	const Animation* GetAnimation() const;
	const float GetStartFrame();
	const float GetEndFrame();

private:
	//const R_Animation* animation;
	std::string			name;
	uint				start;
	uint				end;
	float				speed;
	float				duration;
	float				durationInSeconds;


	float				time;
	float				frame;
	uint				tick;

	bool				loop;
	bool				interruptible;
	const Animation*			clipAnim;
};
#endif // !__ANIMATOR_CLIP_H__
