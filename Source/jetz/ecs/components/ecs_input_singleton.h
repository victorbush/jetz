/*=============================================================================
ecs_transform_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>

#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/**
A singleton component that tracks user input state.
*/
class ecs_input_singleton {
public:
	bool		mouse_down[GLFW_MOUSE_BUTTON_LAST];
	glm::vec2	mouse_pos;
	glm::vec2	mouse_pos_prev;
	bool		key_down[GLFW_KEY_LAST];
};

}   /* namespace jetz */
