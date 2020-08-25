/*=============================================================================
vlk_pipeline.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/pipelines/vlk_pipeline.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_pipeline::vlk_pipeline
	(
	vlk_device&						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	)
	: dev(device),
	render_pass(render_pass),
	extent(extent)
{
}

vlk_pipeline::~vlk_pipeline()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_pipeline::bind(VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */