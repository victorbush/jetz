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

/**

*/
static void shutdown()
{
	glfwTerminate();
}

/**

*/
static void startup()
{
	glfwInit();

	/* For Vulkan, use GLFW_NO_API */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

}

/**
Application entry point.

argc: number of arguments, including name of executable
argv: array of the arguments
*/
int main(int argc, char* argv[])
{
	/*
	Parse command line args 
	*/
	std::vector<std::string> args(argv + 1, argv + argc);
	parse_cmd_line(args);

	/* 
	Seutp logging
	*/
	auto* log = new jetz::log();
	jetz::log::logger = log;

	/* Add simple logging target */
	log->register_target([](const std::string& msg) {
		std::cout << msg;
	});

	LOG_INFO("Logger initialized.");

	/* 
	Setup window 
	*/
	auto* window = new jetz::window(800, 600);

	/* 
	Setup renderer 
	*/
	auto* gpu = new jetz::vlk();

	/* 
	Cleanup 
	*/
	delete gpu;
	delete window;
	delete log;

	return EXIT_SUCCESS;
}
