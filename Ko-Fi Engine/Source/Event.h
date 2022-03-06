#pragma once
struct Event
{
	enum EventType
	{
		play,
		pause,
		unpause,
		stop,
		gameobject_destroyed,
		window_resize,
		file_dropped,
		invalid
	} type;

	union
	{
		struct
		{
			const char* ptr;
		} string;
		struct
		{
			int x, y;
		} point2d;
	};

	Event(EventType type) : type(type)
	{}
};