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
	CONSOLE_LOG("Engine starting ...");
	appLog->AddLog("Engine starting ...\n");

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		switch (state)
		{
			// Allocate the engine --------------------------------------------
		case CREATE:
			CONSOLE_LOG("CREATION PHASE ===============================");
			appLog->AddLog("CREATION PHASE ===============================\n");
			engine = new KoFiEngine(argc, args);
			if (engine != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			CONSOLE_LOG("AWAKE PHASE ===============================");
			appLog->AddLog("AWAKE PHASE ===============================\n");
			if (engine->Awake() == true)
				state = START;
			else
			{
				CONSOLE_LOG("ERROR: Awake failed");
				appLog->AddLog("ERROR: Awake failed\n");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			CONSOLE_LOG("START PHASE ===============================");
			appLog->AddLog("START PHASE ===============================\n");
			if (engine->Start() == true)
			{
				state = LOOP;
				CONSOLE_LOG("UPDATE PHASE ===============================");
				appLog->AddLog("UPDATE PHASE ===============================\n");
			}
			else
			{
				state = FAIL;
				CONSOLE_LOG("ERROR: Start failed");
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
			CONSOLE_LOG("CLEANUP PHASE ===============================");
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
			CONSOLE_LOG("Exiting with errors :(");
			appLog->AddLog("Exiting with errors :(\n");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	CONSOLE_LOG("... Bye! :)\n");
	appLog->AddLog("... Bye! :)\n");

	// Dump memory leaks
	return result;
}