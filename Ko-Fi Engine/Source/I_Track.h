#ifndef __I_TRACK_H__
#define __I_TRACK_H__

class KoFiEngine;
class R_Track;

class I_Track
{
public:
	I_Track(KoFiEngine* engine);
	~I_Track();

	bool Import(const char* path, R_Track* track);

private:
	bool LoadMP3(const char* path, R_Track* track);
	bool LoadWav(const char* path, R_Track* track);
	bool LoadFlac(const char* path, R_Track* track);

private:
	KoFiEngine* engine = nullptr;
};

#endif // !__I_TRACK_H__