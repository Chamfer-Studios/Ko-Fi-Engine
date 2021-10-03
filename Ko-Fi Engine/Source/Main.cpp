#include <stdlib.h>

#include "Defs.h"
#include "Log.h"

#include "Engine.h"

#include "ImGuiAppLog.h"

enum MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

ExampleAppLog* appLog = nullptr;

int main(int argc, char* args[])
{
	KoFiEngine* engine = NULL;
	appLog = new ExampleAppLog();
	LOG("Engine starting ...");
	appLog->AddLog("Engine starting ...\n");

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		switch (state)
		{
			// Allocate the engine --------------------------------------------
		case CREATE:
			LOG("CREATION PHASE ===============================");
			appLog->AddLog("CREATION PHASE ===============================\n");
			engine = new KoFiEngine(argc, args);
			if (engine != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG("AWAKE PHASE ===============================");
			appLog->AddLog("AWAKE PHASE ===============================\n");
			if (engine->Awake() == true)
				state = START;
			else
			{
				LOG("ERROR: Awake failed");
				appLog->AddLog("ERROR: Awake failed\n");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			LOG("START PHASE ===============================");
			appLog->AddLog("START PHASE ===============================\n");
			if (engine->Start() == true)
			{
				state = LOOP;
				LOG("UPDATE PHASE ===============================");
				appLog->AddLog("UPDATE PHASE ===============================\n");
			}
			else
			{
				state = FAIL;
				LOG("ERROR: Start failed");
				appLog->AddLog("ERROR: Start failed\n");
			}
			break;

			// Loop all modules until we are asked to leave ---------------------
		case LOOP:
			if (engine->Update() == false)
				state = CLEAN;
			break;

			// Cleanup allocated memory -----------------------------------------
		case CLEAN:
			LOG("CLEANUP PHASE ===============================");
			appLog->AddLog("CLEANUP PHASE ===============================\n");
			if (engine->CleanUp() == true)
			{
				RELEASE(engine);
				result = EXIT_SUCCESS;
				state = EXIT;
			}
			else
				state = FAIL;

			break;

			// Exit with errors and shame ---------------------------------------
		case FAIL:
			LOG("Exiting with errors :(");
			appLog->AddLog("Exiting with errors :(\n");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	LOG("... Bye! :)\n");
	appLog->AddLog("... Bye! :)\n");

	// Dump memory leaks
	return result;
}