#pragma once

#include "v8.h"
#include "libplatform/libplatform.h"

extern std::unique_ptr<v8::Platform> platform;
extern v8::Isolate::CreateParams createParams;
extern v8::Isolate* isolate;