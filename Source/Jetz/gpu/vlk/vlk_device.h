/*=============================================================================
vlk_device.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_gpu.h"
#include "thirdparty/vma/vma.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

/**
Vulkan logical device.
*/
class vlk_device {

public:

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/


private: 

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_gpu*						gpu;

	/*
	Create/destroy
	*/
	VmaAllocator					allocator;
	VkCommandPool					command_pool;
	VkDevice						handle;					/* Handle for the logical device */
	VkSampler						texture_sampler;
	std::vector<uint32_t>			used_queue_families;	/* Unique set of queue family indices used by this device */

	VkRenderPass					render_pass;
	VkRenderPass					picker_render_pass;

	//_vlk_descriptor_layout_t		material_layout;
	//_vlk_descriptor_layout_t		per_view_layout;

	/*
	Queues and families
	*/
	int								gfx_family_idx;
	VkQueue							gfx_queue;
	int								present_family_idx;
	VkQueue							present_queue;
};

}   /* namespace jetz */
