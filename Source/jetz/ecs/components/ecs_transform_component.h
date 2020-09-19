/*=============================================================================
ecs_transform_component.h
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
A coordinate transformation that can be applied to an entity.
*/
class ecs_transform_component : public ecs_component {

public:

	/*
	Property names
	*/
	const char*			POSITION = "pos";

	/*
	Property data
	*/
	glm::vec3			pos;

	/*-----------------------------------------------------
	ecs_component
	-----------------------------------------------------*/

	virtual void load_lua(lua& script) override;
};

}   /* namespace jetz */
