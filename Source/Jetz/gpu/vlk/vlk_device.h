/*=============================================================================
vlk_device.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_gpu.h"
#include "jetz/gpu/vlk/descriptors/vlk_material_layout.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_layout.h"
#include "thirdparty/vma/vma.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

/**
Vulkan logical device.
*/
class vlk_device {

public:

	vlk_device
		(
		vlk_gpu&							gpu,			/* physical device used by the logical device */
		const std::vector<const char*>&		req_dev_ext,	/* required device extensions */
		const std::vector<const char*>&		req_inst_layers	/* required instance layers */
		);

	~vlk_device();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	Begins recording of a one-time command buffer.
	*/
	VkCommandBuffer begin_one_time_cmd_buf() const;

	/**
	Copies a buffer to an image.
	*/
	void copy_buffer_to_img_now
	(
		VkBuffer				buffer,
		VkImage					image,
		uint32_t				width,
		uint32_t				height
	) const;

	/**
	Creates a shader module.
	*/
	VkShaderModule create_shader(const std::string& file) const;

	/**
	Destroys a shader module.
	*/
	void destroy_shader(VkShaderModule shader) const;

	/**
	Ends recording of one-time command buffer and submits it to the queue.
	*/
	void end_one_time_cmd_buf(VkCommandBuffer cmd_buf) const;

	/* Gets the VMA allocator handle. */
	VmaAllocator get_allocator() const;

	/* Gets the command pool handle. */
	VkCommandPool get_cmd_pool() const;

	/* Gets the physical device being used. */
	vlk_gpu& get_gpu() const;

	/* Gets the Vulkan logical device handle. */
	VkDevice get_handle() const;

	/* Gets the render pass handle. */
	VkRenderPass get_render_pass() const;

	/* Gets the render pass handle for the picker buffer rendering. */
	VkRenderPass get_picker_render_pass() const;

	int									get_gfx_family_idx() const;
	VkQueue								get_gfx_queue() const;
	vlk_material_layout&				get_material_layout() const;
	vlk_per_view_layout&				get_per_view_layout() const;
	int									get_present_family_idx() const;
	VkQueue								get_present_queue() const;
	VkSampler							get_texture_sampler() const;

	void transition_image_layout
	(
		VkImage							image,
		VkFormat						format,
		VkImageLayout					old_layout,
		VkImageLayout					new_layout
	) const;

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/** Creates a memory allocator using the Vulkan Memory Allocation library. */
	void create_allocator();

	/** Creates the command pool. */
	void create_command_pool();

	/** Creates descriptor set layouts. */
	void create_layouts();

	/** Creates the logical device. */
	void create_logical_device
		(
		const std::vector<const char*>&		req_dev_ext,		/* required device extensions */
		const std::vector<const char*>&		req_inst_layers		/* required instance layers */
		);

	/** Creates render pass for the screen picker. Used for determining where the mouse clicked in the editor. */
	void create_picker_render_pass();

	/** Creates render passes. */
	void create_render_pass();

	/** Creates a texture sampler. */
	void create_texture_sampler();

	/** Destroys the memory allocator. */
	void destroy_allocator();

	/** Destroys the command pool. */
	void destroy_command_pool();

	/** Destroys descriptor set layouts. */
	void destroy_layouts();

	/** Destroys the logical device. */
	void destroy_logical_device();

	/** Destroys render pass. */
	void destroy_picker_render_pass();

	/** Destroys render pass. */
	void destroy_render_pass();

	/** Destroys a texture sampler. */
	void destroy_texture_sampler();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_gpu&						gpu;

	/*
	Create/destroy
	*/
	VmaAllocator					allocator;
	VkCommandPool					command_pool;
	VkDevice						handle;					/* Handle for the logical device */
	VkSampler						texture_sampler;
	std::vector<uint32_t>			used_queue_families;	/* Unique set of queue family indices used by this device */

	VkRenderPass					render_pass;
	VkRenderPass					picker_render_pass;

	vlk_material_layout*			material_layout;
	vlk_per_view_layout*			per_view_layout;

	/*
	Queues and families
	*/
	int								gfx_family_idx;
	VkQueue							gfx_queue;
	int								present_family_idx;
	VkQueue							present_queue;
};

}   /* namespace jetz */
