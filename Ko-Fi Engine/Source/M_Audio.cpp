#include "M_Audio.h"
#include "Log.h"
#include "ImGuiAppLog.h"

#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if( error != AL_NO_ERROR)\
	{\
		std::cerr << "OpenAL Error: " << error << " with call for " << #message << std::endl;\
	}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

M_Audio::M_Audio(KoFiEngine* engine)
{
	name = "Audio";
	this->engine = engine;

	context = nullptr;
	device = nullptr;
}

M_Audio::~M_Audio()
{
    
}

bool M_Audio::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool M_Audio::Start()
{
	CONSOLE_LOG("Initializing M_Audio System...");
	appLog->AddLog("Initializing M_Audio System...\n");

	device = alcOpenDevice(NULL);
	if (!device)
		CONSOLE_LOG("Failed to get the default device for OpenAL");

	CONSOLE_LOG("OpenAL Device %s", alcGetString(device, ALC_DEVICE_SPECIFIER));

	//ALboolean enumeration;
	//enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT");

	//if (enumeration == AL_FALSE)
	//	CONSOLE_LOG("Enumeration not supported");
	//else
	//	ListM_AudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	context = alcCreateContext(device, nullptr);

	ALboolean bEAX = alIsExtensionPresent("EAX2.0");
	if (bEAX == AL_FALSE)
		CONSOLE_LOG("EAX not supported");

	if (!alcMakeContextCurrent(context))
		CONSOLE_LOG("Failed to make the OpenAL context the current context");

	return true;
}

bool M_Audio::Update(float dt)
{
	return true;
}

bool M_Audio::CleanUp()
{
	return true;
}

void M_Audio::OnNotify(const Event& event)
{

}

void M_Audio::CreateM_AudioListener(float x, float y, float z)
{
	alec(alListener3f(AL_POSITION, x, y, z));
	alec(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));
	ALfloat forwardUpVec[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	alec(alListenerfv(AL_ORIENTATION, forwardUpVec));
}

bool M_Audio::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool M_Audio::LoadConfiguration(Json& configModule)
{
	return true;
}

bool M_Audio::InspectorDraw()
{
	return true;
}

void M_Audio::ListM_AudioDevices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	//LOG("Devices list: ");
	//fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0')
	{
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}