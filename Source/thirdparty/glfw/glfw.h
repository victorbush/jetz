/*=============================================================================
glfw.h

Includes the correct GLFW header based on build configuration.
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/build_config.h"

/*
* Define GLFW renderer 
*/
#ifdef JETZ_BUILD_RENDERER_VULKAN
    #define GLFW_INCLUDE_VULKAN
#else
    #error Renderer not specified in Klink build configuration.
#endif

/*
* Include correct header file
*/
#if _WIN64
    #include "glfw-3.2.1.bin.win64/include/GLFW/glfw3.h"
#else
    #error Unknown platform.
#endif
