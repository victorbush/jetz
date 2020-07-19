/*=============================================================================
window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

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
	Gets the GLFW window handle for this window.
	*/
	GLFWwindow*						get_hndl();

private:


	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
	GLFWUserData					_glfw_user_data;
	GLFWwindow*						_hndl;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/
};

}   /* namespace jetz */
