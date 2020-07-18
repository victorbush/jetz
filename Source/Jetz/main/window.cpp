/*=============================================================================
window.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory>

#include "thirdparty/glfw/glfw.h"
#include "jetz/main/window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC STATIC FUNCTIONS
=============================================================================*/

window* window::create(int width, int height)
{
	window* w = new window();

	/* create the GLFW window */
	w->_hndl = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	/* setup user data pointer for the window */
	memset(&w->_glfw_user_data, 0, sizeof(w->_glfw_user_data));
	glfwSetWindowUserPointer(w->_hndl, &w->_glfw_user_data);

	/* set GLFW window callbacks */
	glfwSetFramebufferSizeCallback(w->_hndl, framebufferResizeCallback);
	glfwSetKeyCallback(w->_hndl, keyCallback);
	glfwSetCursorPosCallback(w->_hndl, cursorPosCallback);
	glfwSetMouseButtonCallback(w->_hndl, mouseButtonCallback);
	glfwSetCharCallback(w->_hndl, charCallback);
	glfwSetScrollCallback(w->_hndl, scrollCallback);

	return w;
}

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

/**
Destrcutor
*/
window::~window()
{
	/* cleanup window */
	glfwDestroyWindow(_hndl);
	_hndl = nullptr;
}

GLFWwindow * window::get_hndl()
{
	return _hndl;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

/**
Constructor
*/
window::window()
{
}

}   /* namespace Klink */
