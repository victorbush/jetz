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

static void charCallback(GLFWwindow* window, unsigned int c)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	//	&& data->InputManager != nullptr)
	//{
	//	data->InputManager->OnChar(window, c);
	//}
	//else
	//{
	//	throw std::runtime_error("Missing GLFW window user data.");
	//}
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	// && data->InputManager)
	//{
	//	data->InputManager->OnKey(window, key, scancode, action, mods);
	//}
	//else
	//{
	//	throw std::runtime_error("Missing GLFW window user data.");
	//}
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	// && data->InputManager != nullptr)
	//{
	//	data->InputManager->OnMouseCursor(window, xpos, ypos);
	//}
	//else
	//{
	//	throw std::runtime_error("Missing GLFW window user data.");
	//}
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	// && data->InputManager != nullptr)
	//{
	//	data->InputManager->OnMouseButton(window, button, action, mods);
	//}
	//else
	//{
	//	throw std::runtime_error("Missing GLFW window user data.");
	//}
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	//	&& data->InputManager != nullptr)
	//{
	//	data->InputManager->OnScroll(window, xoffset, yoffset);
	//}
	//else
	//{
	//	throw std::runtime_error("Missing GLFW window user data.");
	//}
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

window::window(int width, int height) :
	_gpu_window(nullptr),
	_hndl(nullptr)
{
	/* create the GLFW window */
	_hndl = glfwCreateWindow(width, height, "Jetz", nullptr, nullptr);
	_width = width;
	_height = height;

	/* setup user data pointer for the window */
	glfwSetWindowUserPointer(_hndl, this);

	/* set GLFW window callbacks */
	glfwSetFramebufferSizeCallback(_hndl, framebuffer_resize_callback);
	glfwSetKeyCallback(_hndl, keyCallback);
	glfwSetCursorPosCallback(_hndl, cursorPosCallback);
	glfwSetMouseButtonCallback(_hndl, mouseButtonCallback);
	glfwSetCharCallback(_hndl, charCallback);
	glfwSetScrollCallback(_hndl, scrollCallback);
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

void window::on_resize(int width, int height)
{
	_width = (uint32_t)width;
	_height = (uint32_t)height;
	this->_gpu_window->resize(_width, _height);
}

/*=============================================================================
PRIVATE STATIC METHODS
=============================================================================*/

void window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	auto data = (jetz::window*)glfwGetWindowUserPointer(window);
	data->on_resize(width, height);
}

}   /* namespace jetz */
