#define STB_IMAGE_IMPLEMENTATION
#include "hud.h"
#include "stb_image.h"
#include "nlui_utils.h"
#include <filesystem>
#include <SDL.h>
#include <SDL_video.h>
#include "nlfuncs.h"

static struct
{
	int ScreenWidth;
	int ScreenHeight;
	SDL_Window* window;
} g_DisplayInfo;

void NLUtils::Update(SDL_Window *window)
{
	g_DisplayInfo.window = window;
	SDL_GetWindowSize(window, &g_DisplayInfo.ScreenWidth, &g_DisplayInfo.ScreenHeight);
	GetScreenAspectRatio();
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool NLUtils::LoadTextureFromFile(const char* pszfilename, gltexture_s *pTexture)
{
	static char filename[512];
	sprintf(filename, "%s/%s/%s", std::filesystem::current_path().string().c_str(), gEngfuncs.pfnGetGameDirectory(), pszfilename);

	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	pTexture->texture = image_texture;
	pTexture->original_width = pTexture->width = image_width;
	pTexture->original_height = pTexture->height = image_height;

	return true;
}

ImVec2 NLUtils::CalcButtonSize(const char* pszName)
{
	float x = (ImGui::CalcTextSize(pszName).x + ImGui::GetStyle().FramePadding.x * 2);
	float y = (ImGui::CalcTextSize(pszName).y + ImGui::GetStyle().FramePadding.y * 2);
	return ImVec2(x, y);
}

aspectratio_e NLUtils::GetScreenAspectRatio()
{
	int ratio = ((float)g_DisplayInfo.ScreenHeight / (float)g_DisplayInfo.ScreenWidth) * 300.0f;

	if (ratio >= 158 && ratio <= 168)
		return AR_16_9;
	if (ratio == 225)
		return AR_4_3;
	if (ratio == 187)
		return AR_16_9;
	if (ratio == 300)
		return AR_1_1;

	return AR_UNKNOWN;
}

void NLUtils::Scaling(int* w, int* h)
{
	if (!g_DisplayInfo.window || (!w && !h))
		return;

		// scale for screen sizes
	float xscale = g_DisplayInfo.ScreenWidth / (float)640.0f;
	float yscale = g_DisplayInfo.ScreenHeight / (float)480.0f;

	if (w)
	{
		*w *= xscale;
	}
	if (h)
	{
		*h *= xscale;
	}
}

void NLUtils::Scaling(float* w, float* h)
{
	if (!g_DisplayInfo.window || (!w && !h))
		return;

	aspectratio_e aspectratio = GetScreenAspectRatio();

	if (w)
	{
		float factor = (float)g_DisplayInfo.ScreenWidth;
		switch (aspectratio)
		{
		case AR_16_9:
			factor /= 880.0f;
			break;
		case AR_16_10:
			factor /= 768.0f;
			break;
		case AR_1_1:
			factor /= 480.0f;
			break;
		default:
		case AR_4_3:
			factor /= 450.0f;
			break;
		}

		*w *= factor;
	}
	if (h)
	{
		float factor = (float)g_DisplayInfo.ScreenHeight;

		switch (aspectratio)
		{
		case AR_16_9:
			factor /= 852.0f;
			break;
		case AR_16_10:
			factor /= 768.0f;
			break;
		case AR_1_1:
			factor /= 480.0f;
			break;
		default:
		case AR_4_3:
			factor /= 450.0f;
			break;
		}

		*h *= factor;
	}
}

void NLUtils::GetScreenSize(int* w, int* h)
{
	SDL_GetWindowSize(g_DisplayInfo.window, w, h);
}

float NLUtils::ScaleFont()
{
	int w = 0.0f, h = 0.0f;

	NLUtils::GetScreenSize(&w, &h);
	float scale = (w + h);

	if (NLUtils::GetScreenAspectRatio() == AR_16_9)
	{
		scale /= 2160.0f;
	}
	if (NLUtils::GetScreenAspectRatio() == AR_4_3)
	{
		if (w == 640 && h == 480)
		{
			scale = 0;
		}
		else
			scale /= 2048.0f;
	}

	scale = std::clamp(scale, 0.52f, 20.0f);

	return scale;
}

void gltexture_t::Scale()
{
	int w = 0.0f, h = 0.0f;

	NLUtils::GetScreenSize(&w, &h);
	float scale = (w + h);

	if (NLUtils::GetScreenAspectRatio() == AR_16_9)
	{
		scale /= 3840.0f;
	}
	if (NLUtils::GetScreenAspectRatio() == AR_4_3)
	{
		scale /= 2480.0f;
	}

	width = original_width * std::clamp(scale, 0.52f, 20.0f);
	height = original_height * std::clamp(scale, 0.52f, 20.0f);
}

std::map<string, viewmodelinfo_s> g_ViewModelInfoMap;
viewmodelinfo_s* g_pCurrentViewModelInfo;

void ParseViewModelInfo()
{
	static char token[512];
	char *pfile, *afile;
	pfile = afile = reinterpret_cast<char*>(gEngfuncs.COM_LoadFile("resource/nlui/caminfo.txt", 5, 0));

	if (!pfile || !afile)
		return;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		if (!stricmp("name", token))
		{
			viewmodelinfo_s info;
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			info.name = token;
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			if (token[0] == '{')
			{
				while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
				{
					if (token[0] == '}')
						break;

					if (!stricmp("cambone", token))
					{
						pfile = gEngfuncs.COM_ParseFile(pfile, token);
						info.iCamBone = atoi(token);
					}
					else if (!stricmp("camscale", token))
					{
						pfile = gEngfuncs.COM_ParseFile(pfile, token);
						info.flCamScale = atof(token);
					}
				}
			}
			g_ViewModelInfoMap.insert(std::make_pair(info.name, info));
		}
	}

	gEngfuncs.COM_FreeFile(afile);
}