#include "cl_dll.h"
#include "cdll_int.h"
#include <SDL2/SDL.h>
#include "../../subhook/subhook.h"
#include "PlatformHeaders.h"
#include <assert.h>

subhook::Hook foo_hook;

typedef void (*pSDL_GL_SwapWindow)(SDL_Window* window);

void SDLCALL HOOKED_SDL_GL_SwapWindow(SDL_Window* window)
{
	gEngfuncs.Con_Printf("Hook Success! \m");
	((pSDL_GL_SwapWindow)foo_hook.GetTrampoline())(window);
}

void* GetSwapWindowPtr()
{
#ifdef WIN32
	HMODULE m_SDL2;
	m_SDL2 = GetModuleHandle("SDL2.dll");
	assert(m_SDL2 != 0);

	return (void*)GetProcAddress(m_SDL2, "SDL_GL_SwapWindow");
#else
	void *handle = dlopen("./sdl2.so", RTLD_NOW);
	void* func = nullptr;
	assert(handle != nullptr);

	func = (void *)dlsym(handle, "SDL_GL_SwapWindow");

	//dlclose(handle);

	return func;
#endif
}

void HookSdl()
{
	foo_hook.Install((void*)GetSwapWindowPtr(), (void*)&HOOKED_SDL_GL_SwapWindow);
}