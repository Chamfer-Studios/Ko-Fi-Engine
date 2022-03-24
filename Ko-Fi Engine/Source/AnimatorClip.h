#ifndef __ANIMATOR_CLIP_H__
#define __ANIMATOR_CLIP_H__

#include <string>
typedef unsigned int uint;
class R_Animation;

class AnimatorClip
{
public:
	AnimatorClip();
	AnimatorClip(const R_Animation* animation, const std::string& name, uint start, uint end, float speed, bool loop);

	const std::string inline GetName() const { return name; }
	const R_Animation* GetAnimation() const { return clipAnim; }
	const float inline GetStartFrame() const { return start; }
	const float inline GetEndFrame() const { return end; }
	const float inline GetDuration() const { return duration; }

	inline void SetName(const char* name) { this->name = name; }
	inline void SetStartFrame(uint start) { this->start = start; }
	inline void SetEndFrame(uint end) { this->end = end; }
	inline void SetDuration(float duration) { this->duration = duration; }

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
	const R_Animation*	clipAnim;
};
#endif // !__ANIMATOR_CLIP_H__
