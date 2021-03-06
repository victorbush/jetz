/*=============================================================================
vlk_util.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory.h>

#include "jetz/gpu/vlk/vlk_gpu.h"
#include "jetz/gpu/vlk/vlk_util.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC STATIC METHODS
=============================================================================*/

bool vlk_util::are_device_extensions_available
	(
	const std::vector<const char*>&		extensions,		/* list of extension names to check */
	vlk_gpu&							gpu				/* GPU to check                     */
	)
{
	uint32_t i, j;
	bool desired_ext_avail = true;

	/* check if specified extensions are available */
	for (i = 0; i < extensions.size(); ++i)
	{
		const char* ext_name = extensions[i];
		bool found = false;

		/* look for this layer in the available layers */
		for (j = 0; j < gpu.ext_properties.size(); ++j)
		{
			VkExtensionProperties ext = gpu.ext_properties[j];

			if (!strncmp(ext_name, ext.extensionName, sizeof(ext.extensionName)))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			desired_ext_avail = false;
			break;
		}
	}

	return (desired_ext_avail);
}

bool vlk_util::are_instance_extensions_available
	(
	const std::vector<const char*>&		extensions
	)
{
	bool desired_extensions_available = true;
	uint32_t i, j;

	/* get number of instance extensions */
	uint32_t available_extensions_cnt = 0;
	vkEnumerateInstanceExtensionProperties(NULL, &available_extensions_cnt, NULL);

	/* get list of instance extensions */
	std::vector<VkExtensionProperties> available_extensions(available_extensions_cnt);
	vkEnumerateInstanceExtensionProperties(NULL, &available_extensions_cnt, available_extensions.data());

	/* check if specified extensions are available */
	for (i = 0; i < extensions.size(); ++i)
	{
		const char* name = extensions[i];
		bool found = false;

		/* look for this layer in the available layers */
		for (j = 0; j < available_extensions_cnt; ++j)
		{
			VkExtensionProperties* ext = &available_extensions[j];
			if (!strncmp(name, ext->extensionName, sizeof(ext->extensionName)))
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			desired_extensions_available = false;
			break;
		}
	}

	return (desired_extensions_available);
}

bool vlk_util::are_instance_layers_available
	(
	const std::vector<const char*>&		layers
	)
{
	bool desired_layers_available = true;

	/* get number of available instance layers */
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, NULL);

	/* get list of available instance layers*/
	VkLayerProperties* available_layers;
	vkEnumerateInstanceLayerProperties(&count, NULL);
	available_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * count);
	vkEnumerateInstanceLayerProperties(&count, available_layers);

	/* check if specified layers are available */
	for (const auto* layer_name : layers)
	{
		bool layer_found = false;

		/* look for this layer in the available layers */
		for (uint32_t j = 0; j < count; ++j)
		{
			VkLayerProperties layer = available_layers[j];

			if (!strncmp(layer_name, layer.layerName, sizeof(layer.layerName)))
			{
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
		{
			desired_layers_available = false;
			break;
		}
	}

	free(available_layers);

	return (desired_layers_available);
}

char* vlk_util::get_texel_addr
	(
	const char*				buffer,
	VkSubresourceLayout		layout,
	VkDeviceSize			texel_size,
	int						x,
	int						y,
	int						z,
	int						layer
	)
{
	return (char*)(buffer
		+ layout.offset
		+ (layer * layout.arrayPitch)
		+ (z * layout.depthPitch)
		+ (y * layout.rowPitch)
		+ (x * texel_size));
}

void vlk_util::insert_image_memory_barrier
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
	)
{
	VkImageMemoryBarrier image_mem_barrier = {};
	image_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_mem_barrier.srcAccessMask = src_access_mask;
	image_mem_barrier.dstAccessMask = dst_access_mask;
	image_mem_barrier.oldLayout = old_img_layout;
	image_mem_barrier.newLayout = new_img_layout;
	image_mem_barrier.image = image;
	image_mem_barrier.subresourceRange = subresource_range;

	vkCmdPipelineBarrier(
		cmd_buf,
		src_stage_mask,
		dst_stage_mask,
		0,
		0, NULL,
		0, NULL,
		1, &image_mem_barrier);
}

void set_image_layout
	(
	VkCommandBuffer			cmd_buf,
	VkImage					image,
	VkImageLayout			old_img_layout,
	VkImageLayout			new_img_layout,
	VkImageSubresourceRange subresource_range,
	VkPipelineStageFlags	src_stage_mask,
	VkPipelineStageFlags	dst_stage_mask
	)
{
	/* Create an image barrier object */
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.oldLayout = old_img_layout;
	image_memory_barrier.newLayout = new_img_layout;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange = subresource_range;

	/*
	Source layouts (old)
	Source access mask controls actions that have to be finished on the old layout
	before it will be transitioned to the new layout.
	*/
	switch (old_img_layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		/*
		Image layout is undefined (or does not matter)
		Only valid as initial layout
		No flags required, listed only for completeness
		*/
		image_memory_barrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		/*
		Image is preinitialized
		Only valid as initial layout for linear images, preserves memory contents
		Make sure host writes have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		/*
		Image is a color attachment
		Make sure any writes to the color buffer have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		/*
		Image is a depth/stencil attachment
		Make sure any writes to the depth/stencil buffer have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		/*
		Image is a transfer source 
		Make sure any reads from the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		/*
		Image is a transfer destination
		Make sure any writes to the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		/*
		Image is read by a shader
		Make sure any shader reads from the image have been finished
		*/
		image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	default:
		/* Other source layouts aren't handled (yet) */
		LOG_FATAL("Unknown image type for transition.");
		break;
	}

	/*
	Target layouts (new)
	Destination access mask controls the dependency for the new image layout
	*/
	switch (new_img_layout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		/*
		Image will be used as a transfer destination
		Make sure any writes to the image have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		/*
		Image will be used as a transfer source
		Make sure any reads from the image have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		/*
		Image will be used as a color attachment
		Make sure any writes to the color buffer have been finished
		*/
		image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		/*
		Image layout will be used as a depth/stencil attachment
		Make sure any writes to depth/stencil buffer have been finished
		*/
		image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		/*
		Image will be read in a shader (sampler, input attachment)
		Make sure any writes to the image have been finished
		*/
		if (image_memory_barrier.srcAccessMask == 0)
		{
			image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	default:
		/* Other source layouts aren't handled (yet) */
		LOG_FATAL("Unknown image type for transition.");
		break;
	}

	/* Put barrier inside command buffer */
	vkCmdPipelineBarrier(
		cmd_buf,
		src_stage_mask,
		dst_stage_mask,
		0,
		0, NULL,
		0, NULL,
		1, &image_memory_barrier);
}


}   /* namespace jetz */
