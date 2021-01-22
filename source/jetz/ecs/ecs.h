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
#include "jetz/ecs/components/ecs_loader_singleton.h"
#include "jetz/main/lua.h"

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

	std::unordered_set<entity_id>::const_iterator cbegin() const;
	std::unordered_set<entity_id>::const_iterator cend() const;
	entity_id create_entity();
	void destroy_all();
	void destroy_entity(entity_id ent);
	bool entity_exists(entity_id ent) const;
	void load_component(entity_id ent, const std::string& component, lua& lua);

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

	/*
	Singleton components
	*/
	ecs_input_singleton		input_singleton;
	ecs_loader_singleton	loader_singleton;

	/*
	Components
	*/
	ecs_component_manager<ecs_model_component> models;
	ecs_component_manager<ecs_transform_component> transforms;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::unordered_map<std::string, ecs_component_manager_intf*> _component_managers;
	std::unordered_set<entity_id> _entities;

	entity_id _next_id;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	entity_id get_next_available_id() const;
};

}   /* namespace jetz */
