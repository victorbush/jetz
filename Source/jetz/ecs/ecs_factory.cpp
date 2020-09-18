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

void world::load(world& world, const std::string& filename)
{
	ecs& ecs = world._ecs;
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
				ecs__load_component(ecs, ent, key, &script);
			}
		}

		/* Pop entities list */
		lua_script__pop(&script, 1);
	}

	/* Process geometry */
	if (lua_script__push(&script, "geometry"))
	{
		boolean loop = lua_script__start_loop(&script);
		while (loop && lua_script__next(&script))
		{
			/* Check type */
			if (!lua_script__push(&script, "type"))
			{
				kk_log__error("Invalid geometry object.");
				continue;
			}

			char type[64];
			if (lua_script__get_string(&script, type, sizeof(type)))
			{
				if (!strncmp(type, "plane", sizeof(type - 1)))
				{
					/* Load plane */
					lua_script__pop(&script, 1);

					struct geo_plane_s* plane = geo__alloc_plane(&world->geo);
					if (!plane)
					{
						kk_log__fatal("Failed to allocate plane.");
					}

					geo_plane__construct(plane, g_gpu);
					geo_plane__load(plane, &script);
				}
			}
			else
			{
				lua_script__pop(&script, 1);
			}
		}

		/* Pop geometry list */
		lua_script__pop(&script, 1);
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
