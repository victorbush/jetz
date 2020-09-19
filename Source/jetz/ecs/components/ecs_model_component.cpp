/*=============================================================================
ecs_model_component.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/components/ecs_model_component.h"
#include "jetz/main/lua.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void ecs_model_component::load_lua(lua& script)
{
	/* Loop through component members */
	bool loop = script.StartLoop();
	while (loop && script.Next())
	{
		/* Get next member name */
		const auto& key = script.GetKey();
		if (!key.valid)
		{
			LOG_ERROR("Expected key.");
			continue;
		}

		/* Material */
		if (key.value == MATERIAL)
		{
			const auto& val = script.GetString();
			if (!val.valid)
			{
				LOG_ERROR("Invalid material filename.");
				continue;
			}

			material_filename = val.value;
			ecs::loader_singleton.materials.insert(val.value);
		}

		/* Model */
		if (key.value == MODEL)
		{
			const auto& val = script.GetString();
			if (!val.valid)
			{
				LOG_ERROR("Invalid model filename.");
			}

			model_filename = val.value;
			ecs::loader_singleton.models.insert(val.value);
		}
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
