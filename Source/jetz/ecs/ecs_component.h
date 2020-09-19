/*=============================================================================
ecs_component.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

#include "jetz/main/log.h"
#include "jetz/main/lua.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs_component {

public:

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	virtual void load_lua(lua& script) = 0;

protected:

	inline static glm::vec3 vec3_from_vec(std::vector<float> from)
	{
		glm::vec3 to;

		if (from.size() != 3)
		{
			LOG_ERROR("Failed to load vec3 - input vector did not have 3 components.");
		}

		to.x = from[0];
		to.y = from[1];
		to.z = from[2];

		return to;
	}
};

}   /* namespace jetz */
