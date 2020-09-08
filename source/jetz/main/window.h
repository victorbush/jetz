/*=============================================================================
window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <functional>

#include "jetz/ecs/systems/ecs_input_system.h"
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

	/**
	Sets the input system to send user inputs to.
	*/
	void set_input_system(ecs_input_system* input_system);

	/**
	Sets the callback function for when the window requests to be closed.
	*/
	void set_window_close_callback(std::function<void(void)> callback);

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
	gpu_window*						_gpu_window;
	GLFWwindow*						_hndl;
	ecs_input_system*				_input_system;
	std::function<void(void)>		_on_window_close;

	uint32_t						_height;
	uint32_t						_width;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Private static methods
	-----------------------------------------------------*/

	static void char_callback(GLFWwindow* window, unsigned int c);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void window_close_callback(GLFWwindow* window);
};

}   /* namespace jetz */
