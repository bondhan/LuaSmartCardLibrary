#pragma once

#define LOG_FILE_NAME	"LuaSCardTrace.log"
#define RUNTIME_ERROR_SW 0x6FFF

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif