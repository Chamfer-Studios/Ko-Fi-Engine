#include "Channel.h"

Channel::Channel()
{
	name = "[NONE]";
}

Channel::Channel(const char* name)
{
	this->name = (name != nullptr) ? name : "[NONE]";
}

bool Channel::HasKeyframes(KeyframeType type) const
{
	switch (type)
	{
	case KeyframeType::POSITION: { return !(positionKeyframes.size() == 1 && positionKeyframes.begin()->first == -1.0); }	break;
	case KeyframeType::ROTATION: { return !(rotationKeyframes.size() == 1 && rotationKeyframes.begin()->first == -1.0); }	break;
	case KeyframeType::SCALE: { return !(scaleKeyframes.size() == 1 && scaleKeyframes.begin()->first == -1.0); }			break;
	}

	return false;
}

bool Channel::HasPositionKeyframes() const
{
	return !(positionKeyframes.size() == 1 && positionKeyframes.begin()->first == -1.0);
}

bool Channel::HasRotationKeyframes() const
{
	return !(rotationKeyframes.size() == 1 && rotationKeyframes.begin()->first == -1.0);
}

bool Channel::HasScaleKeyframes() const
{
	return !(scaleKeyframes.size() == 1 && scaleKeyframes.begin()->first == -1.0);
}