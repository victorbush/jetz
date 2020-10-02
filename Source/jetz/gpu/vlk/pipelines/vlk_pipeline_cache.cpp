/*=============================================================================
vlk_pipeline_cache.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_set.h"
#include "jetz/gpu/vlk/pipelines/vlk_gltf_pipeline.h"
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
	create_imgui_pipeline();
}

vlk_pipeline_cache::~vlk_pipeline_cache()
{
	destroy_imgui_pipeline();
	destory_gltf_pipelines();
	destroy_gltf_layout();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_pipeline_cache::bind_per_view_set(VkCommandBuffer cmd_buf, vlk_frame& frame, vlk_per_view_set* set)
{
	set->bind(cmd_buf, frame, _gltf_layout_handle);
}

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

vlk_imgui_pipeline& vlk_pipeline_cache::get_imgui_pipeline() const
{
	return *_imgui_pipeline;
}

void vlk_pipeline_cache::resize(VkExtent2D extent)
{
	for (auto& pipeline : _gltf_pipelines)
	{
		pipeline.second->resize(extent);
	}

	_imgui_pipeline->resize(extent);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_pipeline_cache::create_gltf_layout()
{
	auto& per_view_layout = _device.get_per_view_layout();
	auto& material_layout = _device.get_material_layout();

	std::vector<VkDescriptorSetLayout> set_layouts = {
		per_view_layout.get_handle(),
		material_layout.get_handle()
	};

	/*
	Push constants
	*/
	VkPushConstantRange pc_vert = {};
	pc_vert.offset = 0;
	pc_vert.size = sizeof(vlk_gltf_push_constant_vertex);
	pc_vert.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// TODO : size and offset must be a multiple of 4
	VkPushConstantRange push_constants[] =
	{
		pc_vert
	};

	/*
	Make sure push constant data fits. Minimum is 128 bytes. Any bigger and
	need to check if device supports.
	https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPushConstantRange.html
	*/
	//auto maxPushConst = _device.GetGPU().GetDeviceProperties().limits.maxPushConstantsSize;
	auto maxPushConst = 128;
	if (sizeof(vlk_gltf_push_constant) > maxPushConst)
	{
		LOG_FATAL("OBJ push constant size greater than max allowed.");
	}

	/*
	Create the pipeline layout
	*/
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

void vlk_pipeline_cache::create_imgui_pipeline()
{
	auto pipe = new jetz::vlk_imgui_pipeline(_device, _render_pass, _extent);
	_imgui_pipeline = sptr<vlk_imgui_pipeline>(pipe);
}

void vlk_pipeline_cache::destroy_gltf_layout()
{
	vkDestroyPipelineLayout(_device.get_handle(), _gltf_layout_handle, nullptr);
}

void vlk_pipeline_cache::destory_gltf_pipelines()
{
	_gltf_pipelines.clear();
}

void vlk_pipeline_cache::destroy_imgui_pipeline()
{
	_imgui_pipeline.reset();
}

}   /* namespace jetz */
