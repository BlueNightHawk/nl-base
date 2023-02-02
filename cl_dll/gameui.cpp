#include "cl_dll.h"
#include "cdll_int.h"
#include "APIProxy.h"

#include "interface.h"
#include "FileSystem.h"
#include "filesystem_utils.h"
#include "igameui.h"

IGameUI* staticGameUIFuncs = nullptr;
CSysModule* m_hStaticGameUIModule = nullptr;
#ifdef WIN32
#define DEFAULT_SO_EXT ".dll"
#else
#define DEFAULT_SO_EXT ".so"
#endif

void COM_FixSlashes(char* pname)
{
	for (char* pszNext = pname; *pszNext; ++pszNext)
	{
		if (*pszNext == '\\')
			*pszNext = '/';
	}
}

void InitGameUI()
{
	char szDllName[512];

	snprintf(szDllName, ARRAYSIZE(szDllName), "valve/cl_dlls/gameui" DEFAULT_SO_EXT);
	COM_FixSlashes(szDllName);
	g_pFileSystem->GetLocalCopy(szDllName);
	m_hStaticGameUIModule = Sys_LoadModule(szDllName);

	auto gameUIFactory = Sys_GetFactory(m_hStaticGameUIModule);

	if (gameUIFactory)
	{
		staticGameUIFuncs = static_cast<IGameUI*>(gameUIFactory(GAMEUI_INTERFACE_VERSION, nullptr));
	}
}

void ShutdownGameUI()
{
	Sys_UnloadModule(m_hStaticGameUIModule);
	m_hStaticGameUIModule = nullptr;
	staticGameUIFuncs = nullptr;
}