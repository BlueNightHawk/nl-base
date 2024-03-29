// reGS_enginehook.cpp: hw.dll hooking
// Used libraries: SPTLib/MinHook

#include "reGS.h"

Utils utils = Utils::Utils(NULL, NULL, NULL);

typedef int (*_build_number)();
_build_number ORIG_build_number = nullptr;

int build_number();
void HookBuildNum();
void UnookBuildNum();

bool HWHook()
{
	void* handle;
	void* base;
	size_t size;

	if (!MemUtils::GetModuleInfo(WHWEXT, &handle, &base, &size))
		return false;

	utils = Utils::Utils(handle, base, size);

	VGuiWrap2_Hook();
	R_Hook();
	GLDraw_Hook();
	HookBuildNum();

	return true;
}

void ShutdownHooks()
{
	VGuiWrap2_UnHook();
	R_UnHook();
	GLDraw_UnHook();
	UnookBuildNum();
}

// reverse-engineered by oxiKKK
// https://github.com/oxiKKK/goldsrc-reversed-build_number-function/blob/main/build_num.h

#define RELEASE_DAY 34995 // Possibly Thursday, October 24, 1996

const char* date = __DATE__;

char* mon[12] =
	{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char mond[12] =
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int build_number()
{
	int m = 0;
	int d = 0;
	int y = 0;
	int build = 0;

	for (m = 0; m < 11; m++)
	{
		if (strnicmp(&date[0], mon[m], 3) == 0)
			break;
		d += mond[m];
	}

	d += atoi(&date[4]) - 1;

	y = atoi(&date[7]) - 1900;

	build = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1)
		build += 1;

	build -= RELEASE_DAY;
	return build;
}

void HookBuildNum()
{
	Hook(build_number);
}

void UnookBuildNum()
{
}