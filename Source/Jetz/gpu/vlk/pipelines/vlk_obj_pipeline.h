/*=============================================================================
vlk_obj_pipeline.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_obj_pipeline : public vlk_pipeline {

public:

	vlk_obj_pipeline
		(
		vlk_device&						device,
		VkRenderPass					render_pass,
		VkExtent2D						extent
		);

	virtual ~vlk_obj_pipeline();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
};

}   /* namespace jetz */