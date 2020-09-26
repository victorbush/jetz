/*=============================================================================
vlk_gltf_pipeline.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline_create_info.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_gltf_pipeline : public vlk_pipeline {

public:

	vlk_gltf_pipeline
		(
		vlk_device&						device,
		VkRenderPass					render_pass,
		VkExtent2D						extent,
		VkPipelineLayout				layout,
		vlk_pipeline_create_info		create_info
		);

	virtual ~vlk_gltf_pipeline();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/
	
	virtual void	resize(VkExtent2D extent) override;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	vlk_pipeline_create_info			_create_info;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_pipeline();
	void destroy_pipeline();
};

}   /* namespace jetz */
