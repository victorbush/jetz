/*=============================================================================
vlk_pipeline.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_pipeline {

public:

	vlk_pipeline
		(
		vlk_device&						device,
		VkRenderPass					render_pass,
		VkExtent2D						extent
		);

	virtual ~vlk_pipeline();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	VkPipelineLayout		get_layout_handle() const;
	void					bind(VkCommandBuffer cmd) const;
	virtual void			resize(VkExtent2D extent) = 0;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Protected variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&						_dev;					/* logical device */
	VkRenderPass					_render_pass;

	/*
	Create/destroy
	*/
	VkPipeline						_handle;
	VkPipelineLayout				_layout;

	/*
	Other
	*/
	VkExtent2D						_extent;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
};

}   /* namespace jetz */
