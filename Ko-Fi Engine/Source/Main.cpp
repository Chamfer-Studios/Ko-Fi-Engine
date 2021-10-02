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

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		switch (state)
		{
			// Allocate the engine --------------------------------------------
		case CREATE:
			LOG("CREATION PHASE ===============================");
			engine = new KoFiEngine(argc, args);
			if (engine != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG("AWAKE PHASE ===============================");
			if (engine->Awake() == true)
				state = START;
			else
			{
				LOG("ERROR: Awake failed");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			LOG("START PHASE ===============================");
			if (engine->Start() == true)
			{
				state = LOOP;
				LOG("UPDATE PHASE ===============================");
			}
			else
			{
				state = FAIL;
				LOG("ERROR: Start failed");
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
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	LOG("... Bye! :)\n");

	// Dump memory leaks
	return result;
}