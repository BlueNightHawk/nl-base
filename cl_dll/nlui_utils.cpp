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

	int sw = 0.0f, sh = 0.0f;

	NLUtils::GetScreenSize(&sw, &sh);

	float xscale, yscale;

	float yfactor = (float)sw / (float)sh;

	xscale = ((float)sw / 2408.0f);
	yscale = ((float)sh / 2408.0f) * yfactor;

	if (w)
		*w *= xscale;
	if (h)
		*h *= yscale;
}

void NLUtils::Scaling(float* w, float* h)
{
	if (!g_DisplayInfo.window || (!w && !h))
		return;

	aspectratio_e aspectratio = GetScreenAspectRatio();

	int sw = 0.0f, sh = 0.0f;

	NLUtils::GetScreenSize(&sw, &sh);

	float xscale, yscale;

	float yfactor = (float)sw / (float)sh;
	xscale = ((float)sw / 2408.0f);
	yscale = ((float)sh / 2408.0f) * yfactor;

	if (w)
		*w *= xscale;
	if (h)
		*h *= yscale;
}

void NLUtils::GetScreenSize(int* w, int* h)
{
	SDL_GetWindowSize(g_DisplayInfo.window, w, h);
}

float NLUtils::ScaleFont()
{
	int w = 0.0f, h = 0.0f;

	NLUtils::GetScreenSize(&w, &h);

	float xscale, yscale;

	float yfactor = (float)w / (float)h;

	xscale = ((float)w / 2408.0f);
	yscale = ((float)h / 2408.0f) * yfactor;

	return ((xscale)) * 1.55f;
}

void gltexture_t::Scale(float flScale)
{
	int w = 0.0f, h = 0.0f;

	NLUtils::GetScreenSize(&w, &h);

	float xscale, yscale;

	float yfactor = (float)w / (float)h;

	xscale = ((float)w / 2408.0f);
	yscale = ((float)h / 2408.0f) * yfactor;

	width = original_width * xscale;
	height = original_height * yscale;

	width *= flScale;
	height *= flScale;
}


bool gltexture_t::LoadTexture(const char* p)
{
	return NLUtils::LoadTextureFromFile(p, this);
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