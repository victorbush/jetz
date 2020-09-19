/*=============================================================================
ecs_model_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>

#include "jetz/ecs/ecs_component.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs_model_component : public ecs_component {

public:

	/*
	Property names
	*/
	const char*			MODEL = "model";
	const char*			MATERIAL = "material";

	/*
	Property data
	*/
	std::string			model_filename;
	std::string			material_filename;

	/*-----------------------------------------------------
	ecs_component
	-----------------------------------------------------*/

	virtual void load_lua(lua& script) override;
};

}   /* namespace jetz */
