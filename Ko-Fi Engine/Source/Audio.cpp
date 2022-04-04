#include "Audio.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "Globals.h"

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

Audio::Audio(KoFiEngine* engine)
{
	name = "Audio";
	this->engine = engine;

	context = nullptr;
	device = nullptr;
}

Audio::~Audio()
{
	CleanUp();
}

bool Audio::Awake(Json configModule)
{
	bool ret = true;

	ret = LoadConfiguration(configModule);

	return ret;
}

bool Audio::Start()
{
	CONSOLE_LOG("Initializing Audio System...");
	appLog->AddLog("Initializing Audio System...\n");

	device = alcOpenDevice(NULL);
	if (!device)
		CONSOLE_LOG("Failed to get the default device for OpenAL");

	CONSOLE_LOG("OpenAL Device", alcGetString(device, ALC_DEVICE_SPECIFIER));

	ListAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	ALboolean enumeration;
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT");
	if (enumeration == AL_FALSE)
		CONSOLE_LOG("Enumeration not supported");

	context = alcCreateContext(device, nullptr);

	ALboolean bEAX = alIsExtensionPresent("EAX2.0");
	if (bEAX == AL_FALSE)
		CONSOLE_LOG("EAX not supported");

	if (!alcMakeContextCurrent(context))
		CONSOLE_LOG("Failed to make the OpenAL context the current context");

	return true;
}

bool Audio::Update(float dt)
{
	return true;
}

bool Audio::CleanUp()
{
	engine = nullptr;

	if (device)
		RELEASE(device);

	if (context)
		RELEASE(context);

	return true;
}

void Audio::OnNotify(const Event& event)
{

}

void Audio::CreateAudioListener(float x, float y, float z)
{
	alec(alListener3f(AL_POSITION, x, y, z));
	alec(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));
	ALfloat forwardUpVec[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	alec(alListenerfv(AL_ORIENTATION, forwardUpVec));
}

bool Audio::SaveConfiguration(Json& configModule) const
{
	return true;
}

bool Audio::LoadConfiguration(Json& configModule)
{
	return true;
}

bool Audio::InspectorDraw()
{
	return true;
}

void Audio::ListAudioDevices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	//LOG("Devices list: ");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0')
	{
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}