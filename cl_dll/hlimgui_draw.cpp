#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl2.h"
#include "backends/imgui_impl_sdl.h"

SDL_Window *window;

int ProcessEvent(void* userdata, SDL_Event* event)
{
	return ImGui_ImplSDL2_ProcessEvent(event);
}

void InitImgui()
{
    int i = 0;
    while(!window)
    {  
	    window = SDL_GetWindowFromID(i);
        i++;   
    }
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL2_Init();

	SDL_AddEventWatch(&ProcessEvent, nullptr);
}

void DrawImgui()
{
    ImGuiIO& io = ImGui::GetIO();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

    // Rendering
	ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}
