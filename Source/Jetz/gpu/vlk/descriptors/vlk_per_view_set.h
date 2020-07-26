/*=============================================================================
vlk_per_view_set.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_layout.h"
#include "jetz/main/camera.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_per_view_set {

public:

	vlk_per_view_set
		(
		vlk_per_view_layout& layout
		);

	~vlk_per_view_set();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Binds the descriptor set for the specified frame.
	*/
	void _vlk_per_view_set__bind
		(
		VkCommandBuffer					cmd_buf,
		const vlk_frame&				frame,
		VkPipelineLayout				pipelineLayout
		);

	/**
	Updates data in the per-view UBO for the specified frame.
	*/
	void _vlk_per_view_set__update
		(
		const vlk_frame&				frame,
		const camera&					camera,
		VkExtent2D						extent
		);

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/** Creates buffers for the set. */
	void create_buffers();

	/** Creates the descriptor sets. */
	void create_sets();

	/** Destroys buffers for the set. */
	void destroy_buffers();

	/** Destroys the descriptor sets. */
	void destroy_sets();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_per_view_layout&			layout;

	/*
	Create/destroy
	*/
	std::vector<vlk_buffer*>		buffers;
	std::vector<VkDescriptorSet>	sets;
};

}   /* namespace jetz */
