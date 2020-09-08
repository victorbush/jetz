/*=============================================================================
ecs.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <unordered_map>
#include <unordered_set>

#include "jetz/ecs/ecs_.h"
#include "jetz/ecs/ecs_component_manager.h"
#include "jetz/ecs/components/ecs_model_component.h"
#include "jetz/ecs/components/ecs_transform_component.h"
#include "jetz/ecs/components/ecs_input_singleton.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
ECS CORE
=============================================================================*/

class ecs {

public:

	ecs();
	~ecs();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	entity_id create_entity();
	void destory_entity(entity_id ent);
	bool entity_exists(entity_id ent) const;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

	static ecs_input_singleton	input_singleton;

	/*
	Components
	*/
	ecs_component_manager<ecs_model_component> models;
	ecs_component_manager<ecs_transform_component> transforms;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::unordered_set<entity_id> _entities;

	entity_id _next_id;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	entity_id get_next_available_id() const;
};

}   /* namespace jetz */
