#include "reGS.h"
#include "hud.h"
#include "cl_util.h"
#include "cvardef.h"
#include "SDL_opengl.h"
#include <cmath>

// FIXME: shorten these?
typedef struct mspriteframe_s
{
	int width;
	int height;
	float up, down, left, right;
	int gl_texturenum;
} mspriteframe_t;

typedef struct
{
	int numframes;
	float* intervals;
	mspriteframe_t* frames[1];
} mspritegroup_t;

typedef struct
{
	int type;
	mspriteframe_t* frameptr;
} mspriteframedesc_t;

typedef struct
{
	int type;
	int maxwidth;
	int maxheight;
	int numframes;
	float beamlength; // remove?
	void* cachespot;  // remove?
	mspriteframedesc_t frames[1];
} msprite_t;


struct SPRITELIST
{
	model_t* pSprite;
	char* pName;
	int frameCount;
};

msprite_t* SPR_Pointer(SPRITELIST* pList);

#undef SPR_EnableScissor
#undef SPR_DisableScissor
#undef SPR_Frames
#undef SPR_Set

#define SPR_MAX_SPRITES 256
unsigned short gSpritePalette[256];

HSPRITE ghCrosshair = 0;
Rect gCrosshairRc = {};
int gCrosshairR = 0;
int gCrosshairG = 0;
int gCrosshairB = 0;

static msprite_t* gpSprite = nullptr;

static int gSpriteCount = 0;
static SPRITELIST* gSpriteList = nullptr;

static int scissor_x = 0, scissor_y = 0, scissor_width = 0, scissor_height = 0;
static qboolean giScissorTest = false;

void EnableScissorTest(int x, int y, int width, int height)
{
	x = std::clamp(x, 0, ScreenWidth);
	y = std::clamp(y, 0, ScreenHeight);
	width = std::clamp(width, 0, ScreenWidth - x);
	height = std::clamp(height, 0, ScreenHeight - y);

	scissor_x = x;
	scissor_width = width;
	scissor_y = y;
	scissor_height = height;

	giScissorTest = true;
}

void DisableScissorTest(void)
{
	scissor_x = 0;
	scissor_width = 0;
	scissor_y = 0;
	scissor_height = 0;

	giScissorTest = false;
}


msprite_t* SPR_Pointer(SPRITELIST* pList)
{
	return (msprite_t*)pList->pSprite->cache.data;
}

int SPR_Frames(HSPRITE hSprite)
{
	SPRITELIST* sprlist;

	hSprite--;

	if (hSprite < 0 || hSprite >= gSpriteCount)
		return NULL;

	sprlist = &gSpriteList[hSprite];

	if (sprlist)
		return sprlist->frameCount;

	return NULL;
}

/*
int SPR_Height(HSPRITE hSprite, int frame)
{
	SPRITELIST* sprlist;
	mspriteframe_t* sprframe;
	hSprite--;
	if (hSprite < 0 || hSprite >= gSpriteCount)
		return NULL;
	sprlist = &gSpriteList[hSprite];
	if (!sprlist)
		return NULL;
	sprframe = R_GetSpriteFrame(SPR_Pointer(sprlist), frame);
	if (sprframe)
		return sprframe->height;
	return NULL;
}
int SPR_Width(HSPRITE hSprite, int frame)
{
	SPRITELIST* sprlist;
	mspriteframe_t* sprframe;
	hSprite--;
	if (hSprite < 0 || hSprite >= gSpriteCount)
		return NULL;
	sprlist = &gSpriteList[hSprite];
	if (!sprlist)
		return NULL;
	sprframe = R_GetSpriteFrame(SPR_Pointer(sprlist), frame);
	if (sprframe)
		return sprframe->width;
	return NULL;
}
*/
void SPR_Set(HSPRITE hSprite, int r, int g, int b)
{
	SPRITELIST* sprlist = nullptr;

	hSprite--;

	if (hSprite < 0 || hSprite >= gSpriteCount)
		return;

	sprlist = &gSpriteList[hSprite];

	if (sprlist)
	{
		gpSprite = SPR_Pointer(sprlist);

		if (gpSprite)
			glColor4f(r / 255.0, g / 255.0, b / 255.0, 1.0);
	}
}

void SPR_EnableScissor(int x, int y, int width, int height)
{
	EnableScissorTest(x, y, width, height);
}

void SPR_DisableScissor()
{
	DisableScissorTest();
}

void CLDraw_Hook()
{
	// gEngfuncs.pfnSPR_Set = SPR_Set;
}