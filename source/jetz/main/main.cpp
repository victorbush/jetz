/*=============================================================================
main.cpp

Main entry point into game.
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

//#include <cstdlib>
//#include <functional>
#include <iostream>
//#include <stdexcept>
#include <string>
#include <vector>

#include "jetz/main/app.h"
#include "jetz/gpu/gpu.h"
#include "jetz/gpu/vlk/vlk.h"
#include "jetz/main/log.h"
#include "jetz/main/window.h"
#include "thirdparty/glfw/glfw.h"
#include "thirdparty/imgui/imgui.h"

/*=============================================================================
MACROS / CONSTANTS
=============================================================================*/

/*=============================================================================
TYPES
=============================================================================*/

/*=============================================================================
VARIABLES
=============================================================================*/

static jetz::app*			s_app;
static jetz::gpu*			s_gpu;
static jetz::window*		s_window;

static ImGuiContext*		s_imgui_ctx;

/*=============================================================================
METHODS
=============================================================================*/

/**
Parses command line arguments.

args: list of arguments, excluding the name of the executable.
*/
static void parse_cmd_line(const std::vector<std::string>& args)
{
	// TODO

	for (auto arg : args)
	{

	}
}

static void shutdown()
{
	if (s_gpu)
	{
		s_gpu->wait_idle();
	}

	delete s_app;
	delete s_gpu;
	delete s_window;
	glfwTerminate();
}

static void startup()
{
	/*
	Seutp logging
	*/

	/* Add simple logging target */
	jetz::log::logger.register_target([](const std::string& msg) {
		std::cout << msg;
	});

	LOG_INFO("Logger initialized.");

	/*
	Setup GLFW
	*/
	if (glfwInit() != GLFW_TRUE)
	{
		LOG_FATAL("Failed to initialize GLFW.");
	}

	/* For Vulkan, use GLFW_NO_API */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	/*
	Setup window
	*/
	s_window = new jetz::window(800, 600);

	/*
	Setup imgui 
	*/
	s_imgui_ctx = ImGui::CreateContext(nullptr);
	ImGui::SetCurrentContext(s_imgui_ctx);

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
	io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;

	/*
	Setup renderer
	*/
	s_gpu = (jetz::gpu*)new jetz::vlk(*s_window);
	
	/*
	Setup app
	*/
	s_app = new jetz::app(*s_window);
}

/**
Application entry point.

argc: number of arguments, including name of executable
argv: array of the arguments
*/
int main(int argc, char* argv[])
{
	/* Parse command line args */
	std::vector<std::string> args(argv + 1, argv + argc);
	parse_cmd_line(args);

	/* Startup */
	startup();

	/* Main loop */
	while (!s_app->should_exit())
	{
		glfwPollEvents();
		s_app->run_frame();
	}

	/* Shutdown */
	shutdown();

	return EXIT_SUCCESS;
}
