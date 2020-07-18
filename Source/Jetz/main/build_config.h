/*=============================================================================
build_config.h

Contains preprocessor directives to link the correct libraries based on build
configuration.
=============================================================================*/

#pragma once

/*=============================================================================
CONFIG
=============================================================================*/

#define SOLUTION_DIR	__FILE__"/../../.."

#define JETZ_BUILD_RENDERER_VULKAN

/*=============================================================================
LIBS
=============================================================================*/

/*
Different versions of visual studio:
https://stackoverflow.com/questions/70013/how-to-detect-if-im-compiling-code-with-visual-studio-2008
MSVC++ 14.14 _MSC_VER == 1914 (Visual Studio 2017 version 15.7)
MSVC++ 14.13 _MSC_VER == 1913 (Visual Studio 2017 version 15.6)
MSVC++ 14.12 _MSC_VER == 1912 (Visual Studio 2017 version 15.5)
MSVC++ 14.11 _MSC_VER == 1911 (Visual Studio 2017 version 15.3)
MSVC++ 14.1  _MSC_VER == 1910 (Visual Studio 2017 version 15.0)
MSVC++ 14.0  _MSC_VER == 1900 (Visual Studio 2015 version 14.0)
MSVC++ 12.0  _MSC_VER == 1800 (Visual Studio 2013 version 12.0)
MSVC++ 11.0  _MSC_VER == 1700 (Visual Studio 2012 version 11.0)
MSVC++ 10.0  _MSC_VER == 1600 (Visual Studio 2010 version 10.0)
*/

/*-----------------------------------------------------
GLFW
-----------------------------------------------------*/
#if _WIN64

    #if _MSC_VER >= 1900
        #pragma comment(lib, SOLUTION_DIR"/thirdparty/glfw/glfw-3.2.1.bin.win64/lib-vc2015/glfw3.lib")
    #else
        #error Only Visual Studio 2015 and up are supported for 64-bit builds.
    #endif

#else
    #error Unknown platform.
#endif

/*-----------------------------------------------------
Lua
-----------------------------------------------------*/
#if _MSC_VER >= 1910
	#pragma comment (lib, SOLUTION_DIR"/thirdparty/lua/vc15/lua53.lib")
#else
	#error Need to add Lua libraries for other Visual Studio versions.
#endif

/*-----------------------------------------------------
Vulkan - use user property sheets to set the vulkan SDK location
-----------------------------------------------------*/
#pragma comment(lib, "vulkan-1.lib")
