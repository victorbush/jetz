/*=============================================================================
vlk_per_view_layout.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

struct vlk_per_view_ubo {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camera_pos;
};

class vlk_per_view_layout {

public:

	vlk_per_view_layout
		(
		vlk_device& dev
		);

	~vlk_per_view_layout();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	VkDescriptorSetLayout	get_handle() const;
	vlk_device&				get_device() const;
	VkDescriptorPool		get_pool_handle() const;

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/** Creates a descriptor pool for this layout. Descriptor sets are allocated from the pool. */
	void create_descriptor_pool();

	/** Creates the descriptor set layout. */
	void create_layout();

	/** Destroys the descriptor pool. */
	void destroy_descriptor_pool();

	/** Destroys the descriptor set layout. */
	void destroy_layout();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					dev;

	/*
	Create/destroy
	*/
	VkDescriptorSetLayout		handle;
	VkDescriptorPool			pool_handle;
};

}   /* namespace jetz */
