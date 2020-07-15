/*=============================================================================
Main.cpp

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
static void ParseCommandLineArgs(const std::vector<std::string>& args)
{
	// TODO

	for (auto arg : args)
	{

	}
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
	ParseCommandLineArgs(args);

	///* Create engine */
	//Engine = new Klink::Engine(Klink::RendererType::VULKAN);

	///* Create editor */
	//Editor = new KlinkEditor::EditorApp(*Engine);

	///* Run the main loop */
	//Engine->Run(*Editor);

	///* Cleanup */
	//delete Editor;
	//delete Engine;

	return EXIT_SUCCESS;
}
