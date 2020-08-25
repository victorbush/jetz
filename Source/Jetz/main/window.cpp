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

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	//auto data = reinterpret_cast<GLFWUserData*>(glfwGetWindowUserPointer(window));

	//if (data != nullptr
	// && data->Renderer != nullptr)
	//{
	//	data->Renderer->OnWindowResize(window, width, height);
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

window::window(int width, int height)
{
	/* create the GLFW window */
	hndl = glfwCreateWindow(width, height, "Jetz", nullptr, nullptr);
	this->width = width;
	this->height = height;

	/* setup user data pointer for the window */
	memset(&glfw_user_data, 0, sizeof(glfw_user_data));
	glfwSetWindowUserPointer(hndl, &glfw_user_data);

	/* set GLFW window callbacks */
	glfwSetFramebufferSizeCallback(hndl, framebufferResizeCallback);
	glfwSetKeyCallback(hndl, keyCallback);
	glfwSetCursorPosCallback(hndl, cursorPosCallback);
	glfwSetMouseButtonCallback(hndl, mouseButtonCallback);
	glfwSetCharCallback(hndl, charCallback);
	glfwSetScrollCallback(hndl, scrollCallback);
}

window::~window()
{
	/* cleanup window */
	glfwDestroyWindow(this->hndl);
	this->hndl = nullptr;
}

VkResult window::create_surface(VkInstance instance, VkSurfaceKHR* surface) const
{
	/* Caller is responsible for destorying the surface when they are done. */
	return glfwCreateWindowSurface(instance, this->hndl, NULL, surface);
}

uint32_t window::get_height() const
{
	return this->height;
}

void window::set_gpu_window(jetz::gpu_window* window)
{
	this->gpu_window = window;
}

GLFWwindow* window::get_hndl() const
{
	return this->hndl;
}

uint32_t window::get_width() const
{
	return this->width;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
