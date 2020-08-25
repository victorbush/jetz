/*=============================================================================
vlk_imgui_pipeline.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_texture.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline.h"
#include "thirdparty/imgui/imgui.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_imgui_pipeline : public vlk_pipeline {

public:

	vlk_imgui_pipeline
		(
		vlk_device&						device,
		VkRenderPass					render_pass,
		VkExtent2D						extent
		);

	virtual ~vlk_imgui_pipeline() override;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	void render(const vlk_frame& frame, ImDrawData* draw_data);

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/
	
	void create_buffers();
	void create_descriptor_layout();

	/**
	Creates a descriptor pool for this layout. Descriptor sets are allocated
	from the pool.
	*/
	void create_descriptor_pool();

	/**
	Allocates a descriptor set for each possible concurrent frame.
	*/
	void create_descriptor_sets();

	void create_font_texture();
	void create_pipeline();
	void create_pipeline_layout();
	void destroy_buffers();
	void destroy_descriptor_layout();
	void destroy_descriptor_pool();
	void destroy_descriptor_sets();
	void destroy_font_texture();
	void destroy_pipeline();
	void destroy_pipeline_layout();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Create/destroy
	*/
	VkDescriptorPool				descriptor_pool;
	VkDescriptorSetLayout			descriptor_layout;
	std::vector<VkDescriptorSet>	descriptor_sets;
	vlk_texture*					font_texture;
	VkSampler						font_texture_sampler;

	std::vector<vlk_buffer*>		index_buffers;
	std::vector<uint32_t>			index_buffer_sizes;
	std::vector<vlk_buffer*>		vertex_buffers;
	std::vector<uint32_t>			vertex_buffer_sizes;
};

}   /* namespace jetz */
