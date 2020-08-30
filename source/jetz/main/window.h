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
	gpu_window*						_gpu_window;
	GLFWwindow*						_hndl;

	uint32_t						_height;
	uint32_t						_width;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void on_resize(int width, int height);

	/*-----------------------------------------------------
	Private static methods
	-----------------------------------------------------*/

	static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
};

}   /* namespace jetz */
