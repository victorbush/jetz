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
	: _dev(device),
	_render_pass(render_pass),
	_extent(extent),
	_handle(nullptr),
	_layout(nullptr)
{
}

vlk_pipeline::~vlk_pipeline()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_pipeline::bind(VkCommandBuffer cmd) const
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _handle);
}

VkPipelineLayout vlk_pipeline::get_layout_handle() const
{
	return _layout;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
