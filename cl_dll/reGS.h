#pragma once

#include "cl_dll.h"
#include "cdll_int.h"
#include "studio.h"
#include "com_model.h"
#include "PlatformHeaders.h"
#include "../../subhook/subhook.h"
#include "sptlib-stdafx.hpp"
#include "MemUtils.hpp"
#include "Utils.hpp"
#include "reGS_patterns.hpp"
#include "Hooks.hpp"
#include "reGS_enginehook.h"

extern Utils utils = Utils::Utils(NULL, NULL, NULL);

void VGuiWrap2_Hook();
void R_Hook();