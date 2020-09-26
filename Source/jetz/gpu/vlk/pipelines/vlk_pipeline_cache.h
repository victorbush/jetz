/*=============================================================================
vlk_pipeline_cache.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <unordered_map>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/pipelines/vlk_gltf_pipeline.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline_create_info.h"
#include "jetz/main/common.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
typedef size_t vlk_pipeline_hash;

/*=============================================================================
CLASS
=============================================================================*/

class vlk_pipeline_cache {

public:

	vlk_pipeline_cache
		(
		vlk_device&						device,
		VkRenderPass					render_pass,
		VkExtent2D						extent
		);

	~vlk_pipeline_cache();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/
	const vlk_gltf_pipeline&		create_gltf_pipeline(vlk_pipeline_create_info create_info);
	void							resize(VkExtent2D extent);

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&				_device;
	VkExtent2D				_extent;
	VkRenderPass			_render_pass;

	/*
	Create/destory
	*/
	VkPipelineLayout		_gltf_layout_handle;
	std::unordered_map<vlk_pipeline_hash, uptr<vlk_gltf_pipeline>>
							_gltf_pipelines;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void					create_gltf_layout();
	void					destory_gltf_layout();
};

}   /* namespace jetz */
