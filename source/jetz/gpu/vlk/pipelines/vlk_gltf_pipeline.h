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

/*-----------------------------------------------------
Map of the push constant data used in the shader
-----------------------------------------------------*/

/** Vertex shader push constants */
struct vlk_gltf_push_constant_vertex
{
	glm::mat4	model_matrix;	/* 16 * 4 = 64 bytes */
};

/** All push constants */
struct vlk_gltf_push_constant
{
	vlk_gltf_push_constant_vertex	vertex;
};

}   /* namespace jetz */
