/*=============================================================================
vlk_pipeline_cache.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>

#include "jetz/gpu/vlk/pipelines/vlk_pipeline_cache.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_pipeline_cache::vlk_pipeline_cache
	(
	vlk_device&						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	)
	:
	_device(device),
	_extent(extent),
	_render_pass(render_pass),
	_gltf_layout_handle(nullptr)
{
	create_gltf_layout();
}

vlk_pipeline_cache::~vlk_pipeline_cache()
{
	destory_gltf_layout();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

const vlk_gltf_pipeline& vlk_pipeline_cache::create_gltf_pipeline(vlk_pipeline_create_info create_info)
{
	/* Hash pipeline create info and check for a compatible pipeline */
	auto hash = create_info.hash();

	auto existing = _gltf_pipelines.find(hash);
	if (existing != _gltf_pipelines.end())
	{
		/* Found existing compatible pipeline */
		return *existing->second;
	}

	/* No existing pipeline, create one */
	// TODO : Could use pipeline derivations?
	auto raw = new vlk_gltf_pipeline(_device, _render_pass, _extent, _gltf_layout_handle, create_info);
	_gltf_pipelines[hash] = uptr<vlk_gltf_pipeline>(raw);
	return *_gltf_pipelines[hash];
}

void vlk_pipeline_cache::resize(VkExtent2D extent)
{
	for (auto& pipeline : _gltf_pipelines)
	{
		pipeline.second->resize(extent);
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_pipeline_cache::create_gltf_layout()
{
	auto& per_view_layout = _device.get_per_view_layout();
	
	std::vector<VkDescriptorSetLayout> set_layouts = {
		per_view_layout.get_handle()
	};

	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
	layout_info.pSetLayouts = set_layouts.data();
	layout_info.pushConstantRangeCount = 0;
	layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(_device.get_handle(), &layout_info, nullptr, &_gltf_layout_handle) != VK_SUCCESS)
	{
		LOG_ERROR("Failed to create GLTF pipeline layout.");
	}
}

void vlk_pipeline_cache::destory_gltf_layout()
{
	vkDestroyPipelineLayout(_device.get_handle(), _gltf_layout_handle, nullptr);
}

}   /* namespace jetz */
