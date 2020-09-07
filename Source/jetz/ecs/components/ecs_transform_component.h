/*=============================================================================
ecs_transform_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <string>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/**
A coordinate transformation that can be applied to an entity.
*/
struct ecs_transform_component {

	/*
	Property names
	*/
	const char*			POSITION = "pos";

	/*
	Property data
	*/
	glm::vec3			pos;
};

}   /* namespace jetz */
