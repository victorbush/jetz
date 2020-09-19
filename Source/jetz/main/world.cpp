/*=============================================================================
world.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/world.h"
#include "jetz/main/lua.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC STATIC METHODS
=============================================================================*/

void world::load(world& world, const std::string& filename)
{
	ecs& ecs = world._ecs;

	/* Clear any previous state */
	ecs.destory_all();

	/* Load the world script file */
	lua script;

	if (!script.ExecuteFile(filename))
	{
		LOG_ERROR("File does not exist.");
	}

	if (!script.Push("world"))
	{
		LOG_ERROR("Expected world definition.");
	}

	/* Proess entities list */
	if (script.Push("entities"))
	{
		bool loop = script.StartLoop();
		while (loop && script.Next())
		{
			/* Create new entity */
			entity_id ent = ecs.create_entity();
			if (ent == INVALID_ENTITY)
			{
				LOG_FATAL("Failed to allocate new entity.");
			}

			/* Load components */
			bool comp_loop = script.StartLoop();
			while (comp_loop && script.Next())
			{
				/* Get component name */
				auto key = script.GetKey();
				if (!key.valid)
				{
					LOG_ERROR("Expected component name.");
					continue;
				}

				/* Load the component */
				ecs.load_component(ent, key.value, script);
			}
		}

		/* Pop entities list */
		script.Pop(1);
	}
}

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

world::world()
{
}

world::~world()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

ecs& world::get_ecs()
{
	return _ecs;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
