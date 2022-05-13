#include "V8JSLanguageEnvironment.h"

#include <iostream>
#include <fstream>

#include "C_Script.h"

#include "V8Isolate.h"

std::unique_ptr<v8::Platform> platform = nullptr;
v8::Isolate::CreateParams createParams;
v8::Isolate* isolate = nullptr;

V8JSLanguageEnvironment::V8JSLanguageEnvironment(C_Script* _script) : LanguageEnvironment(_script)
{
}

V8JSLanguageEnvironment::~V8JSLanguageEnvironment()
{
}

v8::MaybeLocal<v8::String> V8JSLanguageEnvironment::ReadFile(v8::Isolate* isolate, const char* name)
{
	FILE* file = fopen(name, "rb");
	if (file == NULL) return v8::MaybeLocal<v8::String>();

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (size_t i = 0; i < size;) {
		i += fread(&chars[i], 1, size - i, file);
		if (ferror(file)) {
			fclose(file);
			return v8::MaybeLocal<v8::String>();
		}
	}
	fclose(file);
	v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(
		isolate, chars, v8::NewStringType::kNormal, static_cast<int>(size));
	delete[] chars;
	return result;
}

bool V8JSLanguageEnvironment::ReloadScript()
{
	if (script->s->path == "")
		return true;

	v8::Isolate::Scope isolateScope(isolate);

	// Create a stack-allocated handle scope.
	v8::HandleScope handleScope(isolate);

	// Create a new context.
	context = v8::Context::New(isolate);

	// Enter the context for compiling and running the script.
	v8::Context::Scope contextScope(context);

	v8::TryCatch tryCatch(isolate);

	auto source = ReadFile(isolate, script->s->path.c_str()).ToLocalChecked();

	// Compile the source code.
	v8::Local<v8::Script> script;
	if (!v8::Script::Compile(context, source).ToLocal(&script)) {
		v8::String::Utf8Value error(isolate, tryCatch.Exception());
		appLog->AddLog(*error);
		// The script failed to compile; bail out.
		return true;
	}

	// Run the script to get the result.
	v8::Local<v8::Value> result;
	if (!script->Run(context).ToLocal(&result)) {
		v8::String::Utf8Value error(isolate, tryCatch.Exception());
		appLog->AddLog(*error);
		// The script failed to compile; bail out.
		return true;
	}

	// Convert the result to an UTF8 string and print it.
	v8::String::Utf8Value utf8(isolate, result);
	appLog->AddLog(*utf8);

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
	SafeFunctionCall("Update", dt);

	return true;
}

bool V8JSLanguageEnvironment::PostUpdate(float dt)
{
	return true;
}

bool V8JSLanguageEnvironment::CleanUp()
{
	return true;
}

template<typename ...Args>
bool V8JSLanguageEnvironment::SafeFunctionCall(std::string name, Args ...args)
{
	v8::Local<v8::Object> global = context->Global();
	v8::MaybeLocal<v8::Value> value = global->Get(context, v8::String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());

	if (value->IsFunction()) {
		v8::Local<v8::Function> func = v8::Handle<v8::Function>::Cast(value);

		std::vector<any> vec = { args... };

		v8::Local<v8::Value> v8args[vec.size()];

		for (unsigned int i = 0; i < vec.size(); i++) {
			switch (vec[i].get_type())
			{
			case any::Float:
				v8args[i] = v8::Number::New(isolate, v8::Number::New(isolate, vec[i].get_float()));
				break;
			case any::String:
				v8args[i] = v8::String::New(isolate, v8::String::NewFromUtf8(isolate, vec[i].get_string()));
				break;
			case any::VariantVector:
				v8args[i] = v8::Object::New(isolate, v8::Object::New(isolate, vec[i].get_variant_vector()));
				break;
			default:
				break;
			}
		}

		v8::Local<v8::Value> jsResult = func->Call(global, vec.size(), v8args);
	}

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
		platform = v8::platform::NewDefaultPlatform();
		v8::V8::InitializePlatform(platform.get());
		v8::V8::Initialize();

		createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		isolate = v8::Isolate::New(createParams);
	}
}

void V8JSLanguageEnvironment::KillV8Platform()
{
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}
