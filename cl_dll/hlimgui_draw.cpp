#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl2.h"
#include "backends/imgui_impl_sdl2.h"
#include "nlfuncs.h"
#include "igameui.h"
#include "nlui_main.h"

SDL_Window* window = nullptr;

static bool g_bImGuiInit = false;

int ProcessEvent(void* userdata, SDL_Event* event)
{
	return static_cast<int>(ImGui_ImplSDL2_ProcessEvent(event));
}

static int iActiveKey = 0;

bool CanShowMenu()
{
	if (!staticGameUIFuncs)
		return true;

	if (iActiveKey == 0)
		iActiveKey = staticGameUIFuncs->IsGameUIActive();

	return (staticGameUIFuncs->IsGameUIActive() - iActiveKey) == 0;
}


void InitColorScheme()
{
	int i = 0;
	static char token[512];
	char *pfile, *afile;
	pfile = afile = reinterpret_cast<char*>(gEngfuncs.COM_LoadFile("resource/nlui/colorscheme.txt", 5, 0));

	if (!pfile || !afile)
	{
		ImGui::StyleColorsDark();
		return;
	}
	auto &col = ImGui::GetStyle().Colors;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		if (i >= ImGuiCol_COUNT)
			break;

		if (strlen(token) == 0)
			continue;

		pfile = gEngfuncs.COM_ParseFile(pfile, token);
		col[i].x = atof(token);
		pfile = gEngfuncs.COM_ParseFile(pfile, token);
		col[i].y = atof(token);
		pfile = gEngfuncs.COM_ParseFile(pfile, token);
		col[i].z = atof(token);
		pfile = gEngfuncs.COM_ParseFile(pfile, token);
		col[i].w = atof(token);

		i++;
	}

	gEngfuncs.COM_FreeFile(afile);

	ImGui::GetStyle().FrameBorderSize = 0.04;
	ImGui::GetStyle().WindowBorderSize = 0.2;
}

void InitImgui()
{
	assert(window != nullptr);

	iActiveKey = 0;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, ImGui::GetCurrentContext());
	ImGui_ImplOpenGL2_Init();

	SDL_AddEventWatch(&ProcessEvent, nullptr);

	g_bImGuiInit = true;

	InitColorScheme();

	ImFont* font1 = io.Fonts->AddFontFromFileTTF(((string) "./" + gEngfuncs.pfnGetGameDirectory() + "/resource/nlui/tahoma.ttf").c_str(), 24);

	g_ImGuiManager.Init();
}

void DrawImgui()
{
	if (window == nullptr)
		return;

	if (!g_bImGuiInit)
		InitImgui();

    ImGuiIO& io = ImGui::GetIO();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (CanShowMenu())
	{
		g_ImGuiManager.Update();
	}
    // Rendering
	ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void ShutdownImgui()
{
	g_ImGuiManager.Shutdown();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DelEventWatch(&ProcessEvent, nullptr);

	g_bImGuiInit = false;
	window = nullptr;
}