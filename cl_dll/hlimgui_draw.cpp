#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl2.h"
#include "backends/imgui_impl_sdl.h"
#include "nlfuncs.h"
#include "igameui.h"

SDL_Window* window = nullptr;

static bool g_bImGuiInit = false;

int ProcessEvent(void* userdata, SDL_Event* event)
{
	return ImGui_ImplSDL2_ProcessEvent(event);
}

bool CanShowMenu()
{
	static int iActiveKey = 0;
	if (iActiveKey == 0)
		iActiveKey = staticGameUIFuncs->IsGameUIActive();

	return (staticGameUIFuncs->IsGameUIActive() - iActiveKey) == 0;
}

void InitImgui()
{
	assert(window != nullptr);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, ImGui::GetCurrentContext());
	ImGui_ImplOpenGL2_Init();

	SDL_AddEventWatch(&ProcessEvent, nullptr);

	g_bImGuiInit = true;
}

void DrawImgui()
{
	if (!g_bImGuiInit)
		InitImgui();

    ImGuiIO& io = ImGui::GetIO();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (CanShowMenu())
	{
		ImGui::ShowDemoWindow();
	}
    // Rendering
	ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}
