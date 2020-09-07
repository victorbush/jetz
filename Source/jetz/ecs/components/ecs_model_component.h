/*=============================================================================
ecs_model_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>

#include "jetz/gpu/gpu_material.h"
#include "jetz/gpu/gpu_model.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

struct ecs_model_component {

	gpu_material*		material;
	gpu_model*			model;

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
};

}   /* namespace jetz */
