#include "V8JSLanguageEnvironment.h"

#include <iostream>
#include <fstream>

#include "C_Script.h"

#include "V8Isolate.h"

V8JSLanguageEnvironment::V8JSLanguageEnvironment(C_Script* _script) : LanguageEnvironment(_script)
{
}

V8JSLanguageEnvironment::~V8JSLanguageEnvironment()
{
}

bool V8JSLanguageEnvironment::ReloadScript()
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

		std::string rawSource = std::string(buffer);

		is.close();

		v8::Isolate::Scope isolateScope(isolate);

		// Create a stack-allocated handle scope.
		v8::HandleScope handleScope(isolate);

		// Create a new context.
		context = v8::Context::New(isolate);

		// Enter the context for compiling and running the script.
		v8::Context::Scope contextScope(context);

		v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, rawSource.c_str()).ToLocalChecked();

		// Compile the source code.
		v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

		// Run the script to get the result.
		v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

		// Convert the result to an UTF8 string and print it.
		v8::String::Utf8Value utf8(isolate, result);
		appLog->AddLog(*utf8);

		delete[] buffer;
	}

	return true;
}

void V8JSLanguageEnvironment::Init()
{

}

bool V8JSLanguageEnvironment::Start()
{
	return true;
}

bool V8JSLanguageEnvironment::EventHandler()
{
	return true;
}

bool V8JSLanguageEnvironment::Update(float dt)
{
	return true;
}

bool V8JSLanguageEnvironment::PostUpdate(float dt)
{
	return true;
}

bool V8JSLanguageEnvironment::CleanUp()
{
	isolate->Dispose();

	return true;
}

template<typename ...Args>
bool V8JSLanguageEnvironment::SafeFunctionCall(std::string name, Args ...args)
{
	return true;
}


inspectorVariantType V8JSLanguageEnvironment::GetProperty(std::string name)
{
	return inspectorVariantType();
}

void V8JSLanguageEnvironment::SetProperty(std::string name, inspectorVariantType value)
{
}

void V8JSLanguageEnvironment::OnCollisionEnter(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnCollisionRepeat(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnCollisionExit(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnTriggerEnter(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnTriggerStay(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnTriggerExit(GameObject* go)
{
}

void V8JSLanguageEnvironment::OnRayCastHit()
{
}

void V8JSLanguageEnvironment::InitV8Platform()
{
	if (isolate == nullptr) {
		std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
		v8::V8::InitializePlatform(platform.get());
		v8::V8::Initialize();

		v8::Isolate::CreateParams createParams;
		createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		isolate = v8::Isolate::New(createParams);
	}
}

void V8JSLanguageEnvironment::KillV8Platform()
{
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}
