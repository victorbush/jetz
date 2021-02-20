/*=============================================================================
ecs_light_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "jetz/ecs/ecs_component.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/**
A point light.
*/
class ecs_light_component : public ecs_component {

public:

	/*
	Property names
	*/
	const char*			COLOR = "color";

	/*
	Property data
	*/
	glm::vec3			color;

	/*-----------------------------------------------------
	ecs_component
	-----------------------------------------------------*/

	virtual void load_lua(ecs& ecs, lua& script) override;
};

}   /* namespace jetz */
