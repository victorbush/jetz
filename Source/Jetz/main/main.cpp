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

/*=============================================================================
MACROS / CONSTANTS
=============================================================================*/

/*=============================================================================
TYPES
=============================================================================*/

/*=============================================================================
VARIABLES
=============================================================================*/

jetz::app*			s_app;
jetz::log*			s_log;
jetz::gpu*			s_gpu;
jetz::window*		s_window;

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
	delete s_app;
	delete s_gpu;
	delete s_window;
	delete s_log;
	glfwTerminate();
}

static void startup()
{
	/*
	Seutp logging
	*/
	s_log = new jetz::log();
	jetz::log::logger = s_log;

	/* Add simple logging target */
	s_log->register_target([](const std::string& msg) {
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
	Setup renderer
	*/
	s_gpu = (jetz::gpu*)new jetz::vlk(s_window->get_hndl());

	/*
	Setup app
	*/
	s_app = new jetz::app();
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
