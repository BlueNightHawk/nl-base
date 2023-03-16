#pragma once

#ifdef WIN32
#define HWEXT "hw.dll"
#define WHWEXT L"hw.dll"
#else
#define HWEXT "hw.so"
#define WHWEXT L"hw.so"
#endif

#include "cl_dll.h"
#include "cdll_int.h"
#include "studio.h"
#include "com_model.h"
#include "PlatformHeaders.h"
#include "funchook.h"

extern funchook_t* g_Hook;

#include "sptlib-stdafx.hpp"
#include "MemUtils.hpp"
#include "Utils.hpp"
#include "reGS_patterns.hpp"
#include "Hooks.hpp"
#include "reGS_enginehook.h"



void VGuiWrap2_Hook();
void R_Hook();
void GLDraw_Hook();

void VGuiWrap2_UnHook();
void R_UnHook();
void GLDraw_UnHook();

void ShutdownHooks();