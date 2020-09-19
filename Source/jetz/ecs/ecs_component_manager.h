/*=============================================================================
ecs_component_manager.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <unordered_map>

#include "jetz/ecs/ecs_.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs_component;

class ecs_component_manager_intf {
public:
	virtual ecs_component* create(entity_id ent) = 0;
	virtual void destory(entity_id entity) = 0;
	virtual bool exists(entity_id entity) const = 0;
};

template <typename T>
class ecs_component_manager : ecs_component_manager_intf {

public:

	ecs_component_manager(std::string name)
		: _component_name(name)
	{
	}

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Creates a component for the specified entity (if it doesn't already have one).
	*/
	ecs_component* create(entity_id entity) override
	{
		if (!exists(entity))
		{
			_components.emplace(entity, T());
		}
	
		return &_components[entity];
	}

	/**
	Destroys a component for the spcified entity (if it has one).
	*/
	void destory(entity_id entity) override
	{
		if (!exists(entity))
		{
			return;
		}

		_components.erase(entity);
	}

	/**
	Checks if the specified entity has this component.
	*/
	bool exists(entity_id entity) const override
	{
		auto it = _components.find(entity);
		return it != _components.end();
	}

	/**
	Gets the component for the specified entity (returns NULL if entity doesn't
	have this component).
	*/
	T* get(entity_id entity) const
	{
		if (!exists(entity))
		{
			return nullptr;
		}

		return &_components[entity];
	}

	/**
	Gets the name of this component.
	*/
	const std::string& get_name() /*override*/
	{
		return _component_name;
	}

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::unordered_map<entity_id, T>	_components;
	std::string							_component_name;
};

}   /* namespace jetz */
