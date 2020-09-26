/*=============================================================================
vlk_pipeline_create_info.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/main/utl.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
STRUCT
=============================================================================*/

struct vlk_pipeline_create_info
{
	std::vector<VkVertexInputAttributeDescription>
								vertex_attribute_descriptions;
	std::vector<VkVertexInputBindingDescription>
								vertex_binding_descriptions;

	size_t hash()
	{
		size_t hash = 0;

		/* Hash attribute descriptions */
		for (auto& attr : vertex_attribute_descriptions)
		{
			utl::hash_combine(hash, attr.binding);
			utl::hash_combine(hash, attr.format);
			utl::hash_combine(hash, attr.location);
			utl::hash_combine(hash, attr.offset);
		}

		/* Hash binding descriptions */
		for (auto& bind : vertex_binding_descriptions)
		{
			utl::hash_combine(hash, bind.binding);
			utl::hash_combine(hash, bind.inputRate);
			utl::hash_combine(hash, bind.stride);
		}

		return hash;
	}
};

}   /* namespace jetz */
