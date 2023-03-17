#pragma once

extern ref_params_s g_refparams;

#include <string>
#include <vector>
#include <map>

using std::string;

typedef struct viewmodelinfo_s
{
	string name = "";
	int iCamBone = -1;
	Vector vCamAngle = g_vecZero;
	float flCamScale = 0.0f;
} viewmodelinfo_t;

extern std::map<string, viewmodelinfo_s> g_ViewModelInfoMap;
extern viewmodelinfo_s* g_pCurrentViewModelInfo;

void NL_Init();
void NL_Update();
void NL_Shutdown();

#include "cvardef.h"

extern cvar_s* r_drawlegs;
extern cvar_s* cl_oldbob;
extern cvar_s* cl_camerabob;