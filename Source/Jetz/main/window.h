/*=============================================================================
window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_window.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
struct GLFWUserData {

	int temp;
};

class window {

public:
	
	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Creates a new window.

	@param width The width of the window.
	@param height The height of the window.
	*/
	window(int width, int height);
	~window();

	/**
	Creates a Vulkan surface.

	@param surface The surface pointer to populate with the new surface.
	*/
	VkResult create_surface(VkInstance instance, VkSurfaceKHR* surface) const;

	/**
	Gets the GPU window object that handles rendering for this window.
	*/
	gpu_window* get_gpu_window() const;

	/**
	Gets the window height.
	*/
	uint32_t get_height() const;

	/**
	Gets the GLFW window handle for this window.
	*/
	GLFWwindow* get_hndl() const;

	/**
	Gets the window width.
	*/
	uint32_t get_width() const;

	/**
	Sets the GPU window to use for rendering. The GPU window handles swapchains, rendering, etc.
	*/
	void set_gpu_window(gpu_window* window);

private:


	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
	GLFWUserData					glfw_user_data;
	gpu_window*						gpu_window;
	GLFWwindow*						hndl;

	uint32_t						height;
	uint32_t						width;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/
};

}   /* namespace jetz */
