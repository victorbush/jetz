/*=============================================================================
window.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory>

#include "jetz/gpu/gpu_window.h"
#include "jetz/main/window.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
STATIC FUNCTIONS
=============================================================================*/

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

window::window(int width, int height) :
	_gpu_window(nullptr),
	_hndl(nullptr),
	_input_system(nullptr),
	_on_window_close(nullptr)
{
	/* create the GLFW window */
	_hndl = glfwCreateWindow(width, height, "Jetz", nullptr, nullptr);
	_width = width;
	_height = height;

	/* setup user data pointer for the window */
	glfwSetWindowUserPointer(_hndl, this);

	/* set GLFW window callbacks */
	glfwSetFramebufferSizeCallback(_hndl, framebuffer_resize_callback);
	glfwSetKeyCallback(_hndl, key_callback);
	glfwSetCursorPosCallback(_hndl, cursor_pos_callback);
	glfwSetMouseButtonCallback(_hndl, mouse_button_callback);
	glfwSetCharCallback(_hndl, char_callback);
	glfwSetScrollCallback(_hndl, scroll_callback);
	glfwSetWindowCloseCallback(_hndl, window_close_callback);
}

window::~window()
{
	/* cleanup window */
	glfwDestroyWindow(_hndl);
	_hndl = nullptr;
}

VkResult window::create_surface(VkInstance instance, VkSurfaceKHR* surface) const
{
	/* Caller is responsible for destorying the surface when they are done. */
	return glfwCreateWindowSurface(instance, _hndl, NULL, surface);
}

gpu_window* window::get_gpu_window() const
{
	return _gpu_window;
}

uint32_t window::get_height() const
{
	return _height;
}

void window::set_gpu_window(jetz::gpu_window* window)
{
	_gpu_window = window;
}

void window::set_input_system(ecs_input_system* input_system)
{
	_input_system = input_system;
}

void window::set_window_close_callback(std::function<void(void)> callback)
{
	_on_window_close = callback;
}

GLFWwindow* window::get_hndl() const
{
	return _hndl;
}

uint32_t window::get_width() const
{
	return _width;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

/*=============================================================================
PRIVATE STATIC METHODS
=============================================================================*/

void window::char_callback(GLFWwindow* window, unsigned int c)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_input_system)
	{
		data->_input_system->on_char(c);
	}
}

void window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_input_system)
	{
		data->_input_system->on_mouse_move(xpos, ypos);
	}
}

void window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	data->_width = (uint32_t)width;
	data->_height = (uint32_t)height;

	if (data->_gpu_window)
	{
		data->_gpu_window->resize(data->_width, data->_height);
	}
}

void window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_input_system)
	{
		data->_input_system->on_key(key, scancode, action, mods);
	}
}

void window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_input_system)
	{
		data->_input_system->on_mouse_button(button, action, mods);
	}
}

void window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_input_system)
	{
		data->_input_system->on_mouse_scroll(xoffset, yoffset);
	}
}

void window::window_close_callback(GLFWwindow* window)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	if (data->_on_window_close)
	{
		data->_on_window_close();
	}
}

}   /* namespace jetz */
