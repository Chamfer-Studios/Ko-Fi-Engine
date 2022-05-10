#include "DuktapeJSLanguageEnvironment.h"

#include "C_Script.h"

#include <iostream>
#include <fstream>

DuktapeJSLanguageEnvironment::DuktapeJSLanguageEnvironment(C_Script* _script) : LanguageEnvironment(_script)
{
}

DuktapeJSLanguageEnvironment::~DuktapeJSLanguageEnvironment()
{
}

bool DuktapeJSLanguageEnvironment::ReloadScript()
{
	if (script->s->path == "")
		return true;

    std::ifstream is(script->s->path, std::ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        char* buffer = new char[length];

        // read data as a block:
        is.read(buffer, length);

        is.close();

		duk_push_lstring(ctx, (const char*)buffer, (duk_size_t) length);
        delete[] buffer;

		if (duk_peval(ctx) != 0) {
			appLog->AddLog("Error: %s\n", duk_safe_to_string(ctx, -1));
			return true;
		}
    }

	return true;
}

void DuktapeJSLanguageEnvironment::Init()
{
	ctx = duk_create_heap_default();

	duk_push_c_function(ctx, DuktapeJSLanguageEnvironment::Log, DUK_VARARGS);
	duk_put_global_string(ctx, "Log");
}

bool DuktapeJSLanguageEnvironment::Start()
{
	return true;
}

bool DuktapeJSLanguageEnvironment::EventHandler()
{
	return true;
}

bool DuktapeJSLanguageEnvironment::Update(float dt)
{
	duk_push_global_object(ctx);
	auto update = duk_get_global_string(ctx, "Update");
	if (update != 0) {
		duk_push_number(ctx, dt);
		duk_call(ctx, 1);
		duk_pop(ctx);
	}
	duk_pop(ctx);

	return true;
}

bool DuktapeJSLanguageEnvironment::PostUpdate(float dt)
{
	return true;
}

bool DuktapeJSLanguageEnvironment::CleanUp()
{
	duk_destroy_heap(ctx);

	return true;
}

sol::protected_function DuktapeJSLanguageEnvironment::GetFunction(std::string name)
{
	return sol::protected_function();
}

template<typename ...Args>
bool DuktapeJSLanguageEnvironment::SafeFunctionCall(std::string name, Args ...args)
{
	return true;
}

inspectorVariantType DuktapeJSLanguageEnvironment::GetProperty(std::string name)
{
	return inspectorVariantType();
}

void DuktapeJSLanguageEnvironment::SetProperty(std::string name, inspectorVariantType value)
{
}

void DuktapeJSLanguageEnvironment::OnCollisionEnter(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnCollisionRepeat(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnCollisionExit(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnTriggerEnter(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnTriggerStay(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnTriggerExit(GameObject* go)
{
}

void DuktapeJSLanguageEnvironment::OnRayCastHit()
{
}

duk_ret_t DuktapeJSLanguageEnvironment::Log(duk_context* ctx)
{
	int i;
	int n = duk_get_top(ctx);  /* #args */

	std::string f = "";

	for (i = 0; i < n; i++) {
		f += std::string(duk_to_string(ctx, i));
		f += " ";
	}

	appLog->AddLog(f.c_str());

	return 0;
}
