/*=============================================================================
main.cpp

Main entry point into game.
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

//#include <cstdlib>
//#include <functional>
//#include <iostream>
//#include <stdexcept>
#include <string>
#include <vector>

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
	/* Parse command line args */
	std::vector<std::string> args(argv + 1, argv + argc);
	parse_cmd_line(args);

	/* Setup window */
	auto* window = jetz::window::create(800, 600);

	///* Create engine */
	//Engine = new Klink::Engine(Klink::RendererType::VULKAN);

	///* Create editor */
	//Editor = new KlinkEditor::EditorApp(*Engine);

	///* Run the main loop */
	//Engine->Run(*Editor);

	/* Cleanup */
	delete window;

	return EXIT_SUCCESS;
}
