#pragma once

#include "PlatformHeaders.h"
#include "SDL_opengl.h"
#include <algorithm>
#include <cmath>
#include "imgui.h"

typedef struct SDL_Window SDL_Window;

typedef struct gltexture_s
{
	GLuint texture = 0;

	int original_width = 0;
	int original_height = 0;

	int width = 0;
	int height = 0;

	void Scale();
} gltexture_t;

typedef enum aspectratio_e
{
	AR_16_9 = 0,
	AR_16_10,
	AR_4_3,
	AR_1_1,
	AR_UNKNOWN
} aspectratio_e;

// Algebraically simplified algorithm
#define lerp(a, b, f) a + f*(b - a)

namespace NLUtils
{
bool LoadTextureFromFile(const char* pszfilename, gltexture_s* pTexture);

aspectratio_e GetScreenAspectRatio();
void Scaling(int* w = nullptr, int* h = nullptr);
void Scaling(float* w = nullptr, float* h = nullptr);
void Update(SDL_Window* window);
float ScaleFont();

void GetScreenSize(int* w = nullptr, int* h = nullptr);

ImVec2 CalcButtonSize(const char* pszName);
}