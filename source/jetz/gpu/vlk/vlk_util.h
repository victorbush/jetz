/*=============================================================================
vlk_util.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_util {

public:

	/*-----------------------------------------------------
	Public static methods
	-----------------------------------------------------*/

	/**
	Checks if a specified list of device extensions are available for the
	specified physical device.
	*/
	static bool are_device_extensions_available
		(
		const std::vector<const char*>&		extensions,		/* list of extension names to check */
		vlk_gpu&							gpu				/* GPU to check                     */
		);

	/**
	Checks if a specified list of instance extensions are available.
	*/
	static bool are_instance_extensions_available(const std::vector<const char*>& extensions);

	/**
	Checks if a specified list of instance layers are available.
	*/
	static bool are_instance_layers_available(const std::vector<const char*>& layers);

	/**
	Gets the address of a texel in an image buffer. This is for an uncompressed
	image subresource. If only an offset is desired, pass NULL (0) as the buffer.

	For compressed formats, a different formula is needed. See the Vulkan spec
	for VkSubresourceLayout for details.

	@param buffer The beginning address of the image buffer that is mapped to 
	CPU visible memory. If only an offset is desired, set this to 0.
	@param layout The subresource layout that describes the image.
	@param texel_size The size of an individual texel in the image.
	@param x The x-coordinate.
	@param y The y-coordinate.
	@param z The z-coordinate (for 3D textures, use 0 for 2D textures).
	@param layer The array layer index (use 0 if none).
	*/
	static char* get_texel_addr
		(
		const char*				buffer,
		VkSubresourceLayout		layout,
		VkDeviceSize			texel_size,
		int						x,
		int						y,
		int						z,
		int						layer
		);

	/**
	https://github.com/SaschaWillems/Vulkan
	*/
	static void insert_image_memory_barrier
		(
		VkCommandBuffer			cmd_buf,
		VkImage					image,
		VkAccessFlags			src_access_mask,
		VkAccessFlags			dst_access_mask,
		VkImageLayout			old_img_layout,
		VkImageLayout			new_img_layout,
		VkPipelineStageFlags	src_stage_mask,
		VkPipelineStageFlags	dst_stage_mask,
		VkImageSubresourceRange subresource_range
		);

	/**
	Creates a image memory barrier for changing the layout of an image and puts it into the
	specified command buffer.

	https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanTools.cpp
	*/
	static void set_image_layout
		(
		VkCommandBuffer			cmd_buf,
		VkImage					image,
		VkImageLayout			old_img_layout,
		VkImageLayout			new_img_layout,
		VkImageSubresourceRange subresource_range,
		VkPipelineStageFlags	src_stage_mask,
		VkPipelineStageFlags	dst_stage_mask
		);
};

}   /* namespace jetz */
