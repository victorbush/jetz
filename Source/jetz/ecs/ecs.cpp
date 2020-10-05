/*=============================================================================
ecs.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/ecs_component_manager.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

ecs::ecs()
	:
	_next_id(0),
	models("model"),
	transforms("transform")
{
	_component_managers[models.get_name()] = (ecs_component_manager_intf*)&models;
	_component_managers[transforms.get_name()] = (ecs_component_manager_intf*)&transforms;
}

ecs::~ecs()
{
	destroy_all();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

std::unordered_set<entity_id>::const_iterator ecs::cbegin() const
{
	return _entities.cbegin();
}

std::unordered_set<entity_id>::const_iterator ecs::cend() const
{
	return _entities.cend();
}

entity_id ecs::create_entity()
{
	entity_id id = get_next_available_id();
	if (id == INVALID_ENTITY)
	{
		LOG_ERROR("Failed to create entity.");
		return INVALID_ENTITY;
	}

	_entities.insert(id);
	return id;
}

void ecs::destroy_all()
{
	for (auto it = _entities.begin(); it != _entities.end(); /* No increment */ )
	{
		destroy_entity(*it++);
	}

	_entities.clear();
}

void ecs::destroy_entity(entity_id ent)
{
	for (const auto c : _component_managers)
	{
		c.second->destory(ent);
	}

	_entities.erase(ent);
}

bool ecs::entity_exists(entity_id ent) const
{
	auto it = _entities.find(ent);
	return it != _entities.end();
}

void ecs::load_component(entity_id ent, const std::string& component, lua& lua)
{
	auto it = _component_managers.find(component);
	if (it == _component_managers.end())
	{
		LOG_ERROR_FMT("Unknown component '{0}'.", component);
		return;
	}

	auto comp = _component_managers[component]->create(ent);
	comp->load_lua(*this, lua);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

entity_id ecs::get_next_available_id() const
{
	entity_id next_id = _next_id;
	entity_id last_possible_id = next_id - 1;

	do
	{
		if (next_id != INVALID_ENTITY && !entity_exists(next_id))
		{
			return next_id;
		}

		next_id++;

	} while (next_id != last_possible_id);

	return INVALID_ENTITY;
}

}   /* namespace jetz */
