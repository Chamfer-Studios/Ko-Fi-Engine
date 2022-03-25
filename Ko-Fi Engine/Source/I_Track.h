#ifndef __I_TRACK_H__
#define __I_TRACK_H__

class R_Track;

class I_Track
{
public:
	I_Track();
	~I_Track();

	bool Import(const char* path, R_Track* track);

private:
	bool LoadMP3(const char* path, R_Track* track);
	bool LoadWav(const char* path, R_Track* track);
	bool LoadFlac(const char* path, R_Track* track);
};

#endif // !__I_TRACK_H__