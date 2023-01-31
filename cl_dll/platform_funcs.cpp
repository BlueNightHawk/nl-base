#include "PlatformHeaders.h"

void* PL_GetModuleHandle(const char* file)
{
#ifdef WIN32
	return GetModuleHandle(file);
#else
	return dlopen(file, RTLD_NOW);
#endif
}

void* PL_GetProcAddress(void* handle, const char* proc)
{
#ifdef WIN32
	return GetProcAddress((HMODULE)handle, proc);
#else
	return dlsym(handle, proc);
#endif
}

void PL_FreeLibrary(void* handle)
{
#ifdef WIN32
	FreeLibrary((HMODULE)handle);
#else
	dlclose(handle);
#endif
}