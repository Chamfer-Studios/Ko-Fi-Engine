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
	~AnimatorClip();
	

	inline void SetName(const char* name) { this->name = name; }
	inline void SetAnimation(const R_Animation* clipAnim) { this->clipAnim = clipAnim; }
	inline void SetStartFrame(uint start) { this->start = start; }
	inline void SetEndFrame(uint end) { this->end = end; }
	inline void SetDuration(float duration) { this->duration = duration; }
	inline void SetDurationInSeconds(float durationInSeconds) { this->durationInSeconds = durationInSeconds; }
	inline void SetLoopBool(float loop) { this->loop = loop; }
	inline void SetFinishedBool(bool finished) { this->finished = finished; }

	const std::string inline GetName() const { return name; }
	const inline R_Animation* GetAnimation() const { return clipAnim; }
	const float inline GetStartFrame() const { return start; }
	const float inline GetEndFrame() const { return end; }
	const float inline GetDuration() const { return duration; }
	const float inline GetDurationInSeconds() const { return durationInSeconds; }
	inline bool& GetLoopBool() { return loop; }
	inline bool& GetFinishedBool() { return finished; }

private:
	//const R_Animation* animation;
	std::string			name;
	uint				start;
	uint				end;
	float				speed = 1.0f;
	float				duration;
	float				durationInSeconds;

	float				time;
	float				frame;
	uint				tick;

	bool				loop = false;
	bool				interruptible;
	const R_Animation*	clipAnim;

	bool				finished = true;
};
#endif // !__ANIMATOR_CLIP_H__
