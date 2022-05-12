#pragma once

#include <libplatform/libplatform.h>
#include <v8.h>

extern std::unique_ptr<v8::Platform> platform;
extern v8::Isolate::CreateParams createParams;
extern v8::Isolate* isolate;