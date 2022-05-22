#ifndef __LOG_H__
#define __LOG_H__

#define CONSOLE_LOG(format, ...) Log(__FILE__, __LINE__, format, __VA_ARGS__)
#define KOFI_WARNING(format, ...) WarningLog(__FILE__, __LINE__, format, __VA_ARGS__)
#define KOFI_ERROR(format, ...) ErrorLog(__FILE__, __LINE__, format, __VA_ARGS__)
#define KOFI_DEBUG(format, ...) DebugLog(__FILE__, __LINE__, format, __VA_ARGS__)
#define KOFI_STATUS(format, ...) StatusLog(__FILE__, __LINE__, format, __VA_ARGS__)

void Log(const char file[], int line, const char* format, ...);
void WarningLog(const char file[], int line, const char* format, ...);
void ErrorLog(const char file[], int line, const char* format, ...);
void DebugLog(const char file[], int line, const char* format, ...);
void StatusLog(const char file[], int line, const char* format, ...);

#endif  // !__LOG_H__