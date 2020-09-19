/*=============================================================================
ecs_transform_component.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/components/ecs_transform_component.h"

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

void ecs_transform_component::load_lua(lua& script)
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

		/* Position */
		if (key.value == POSITION)
		{
			const auto& val = script.GetArray<float, 3>();
			if (!val.valid)
			{
				LOG_ERROR("Invalid position.");
				continue;
			}

			pos = vec3_from_vec(val.value);
		}
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
