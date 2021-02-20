/*=============================================================================
ecs_light_component.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/components/ecs_light_component.h"

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

void ecs_light_component::load_lua(ecs& ecs, lua& script)
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

		/* Color */
		if (key.value == COLOR)
		{
			const auto& val = script.GetArray<float, 3>();
			if (!val.valid)
			{
				LOG_ERROR("Invalid color.");
				continue;
			}

			color = vec3_from_vec(val.value);
		}
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
