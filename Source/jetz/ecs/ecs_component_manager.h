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

template <typename T>
class ecs_component_manager {

public:

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Creates a component for the specified entity (if it doesn't already have one).
	*/
	T* create(entity_id entity)
	{
		if (!exists(entity))
		{
			_components.insert(entity);
		}
	
		return _components[entity];
	}

	/**
	Destroys a component for the spcified entity (if it has one).
	*/
	void destory(entity_id entity)
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
	bool exists(entity_id entity) const
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

		return _components[entity];
	}

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::unordered_map<entity_id, T> _components;
};

}   /* namespace jetz */
