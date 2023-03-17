#pragma once

extern ref_params_s g_refparams;

void NL_Init();
void NL_Update();
void NL_Shutdown();

#include "cvardef.h"

extern cvar_s* r_drawlegs;
extern cvar_s* cl_oldbob;
extern cvar_s* cl_camerabob;