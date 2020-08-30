/*=============================================================================
vlk_window.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>

#include "jetz/gpu/gpu_window.h"
#include "jetz/gpu/vlk/vlk_window.h"
#include "jetz/gpu/vlk/vlk_util.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"
#include "jetz/main/utl.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
DECLARATIONS
=============================================================================*/

/**
Gets the pixel color value at the specified coordinate.

https://github.com/SaschaWillems/Vulkan/blob/master/examples/screenshot/screenshot.cpp
*/
static int read_pixel
	(
	vlk_device& dev,
	VkImage						src_image,
	VkFormat					src_format,
	uint32_t					width,
	uint32_t					height,
	float						x,
	float						y
	);

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_window::vlk_window(vlk_device& device, VkInstance vkInstance, VkSurfaceKHR surface, uint32_t width, uint32_t height)
	: instance(vkInstance),
	dev(device),
	surface(surface),
	gpu_window(width, height)
{
	create_swapchain(width, height);
	create_pipelines();
	create_descriptors();
}

vlk_window::~vlk_window()
{
	destroy_descriptors();
	destroy_pipelines();
	destroy_swapchain();
	destroy_surface();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_window::begin_frame(vlk_frame& frame, const camera& camera)
{
	/* Setup render pass, command buffer, etc. */
	swapchain->begin_frame(frame);

	/* Setup per-view descriptor set data */
	per_view_set->update(frame, camera, swapchain->get_extent());
}

void vlk_window::end_frame(const vlk_frame& frame)
{
	/* End render pass, submit command buffer, preset swapchain */
	swapchain->end_frame(frame);
}

int vlk_window::get_picker_id(const vlk_frame& frame, float x, float y)
{
	return read_pixel(
		dev,
		swapchain->get_picker_image(frame),
		VK_FORMAT_B8G8R8A8_UNORM,
		swapchain->get_extent().width,
		swapchain->get_extent().height,
		x, y);
}

void vlk_window::render_imgui(vlk_frame& frame, ImDrawData* draw_data)
{
	/* Draw imgui */
	imgui_pipeline->render(frame, draw_data);
}

void vlk_window::resize(uint32_t width, uint32_t height)
{
	swapchain->recreate(width, height);

	// TODO : Dynamic state instead of recreating pipelines
	destroy_pipelines();
	create_pipelines();
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_window::create_descriptors()
{
	per_view_set = new jetz::vlk_per_view_set(dev.get_per_view_layout());
}

void vlk_window::create_pipelines()
{
	imgui_pipeline = new jetz::vlk_imgui_pipeline(dev, dev.get_render_pass(), swapchain->get_extent());
}

void vlk_window::create_swapchain(uint32_t width, uint32_t height)
{
	VkExtent2D extent;
	extent.width = width;
	extent.height = height;

	swapchain = new jetz::vlk_swapchain(dev, surface, extent);
}

void vlk_window::destroy_descriptors()
{
	delete per_view_set;
	per_view_set = nullptr;
}

void vlk_window::destroy_pipelines()
{
	delete imgui_pipeline;
	imgui_pipeline = nullptr;
}

void vlk_window::destroy_surface()
{
	vkDestroySurfaceKHR(instance, surface, NULL);
}

void vlk_window::destroy_swapchain()
{
	delete swapchain;
	swapchain = nullptr;
}

/*=============================================================================
STATIC FUNCTIONS
=============================================================================*/

static int read_pixel
	(
	vlk_device&					dev,
	VkImage						src_image,
	VkFormat					src_format,
	uint32_t					width,
	uint32_t					height,
	float						x,
	float						y
	)
{
	bool supports_blit = true;

	/* Check blit support for source and destination */
	VkFormatProperties format_props;

	/* Check if the device supports blitting from optimal images (the swapchain images are in optimal format) */
	vkGetPhysicalDeviceFormatProperties(dev.get_gpu().get_handle(), src_format, &format_props);
	if (!(format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
	{
		//log__dbg("Device does not support blitting from optimal tiled images, using copy instead of blit.");
		supports_blit = false;
	}

	/* Check if the device supports blitting to linear images */
	vkGetPhysicalDeviceFormatProperties(dev.get_gpu().get_handle(), VK_FORMAT_R8G8B8A8_UNORM, &format_props);
	if (!(format_props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
	{
		//log__dbg("Device does not support blitting to linear tiled images, using copy instead of blit.");
		supports_blit = false;
	}

	/*
	Create the linear tiled destination image to copy to and to read the memory from.
	Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ.
	*/
	VkImageCreateInfo image_ci = {};
	image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_ci.imageType = VK_IMAGE_TYPE_2D;
	image_ci.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_ci.extent.width = width;
	image_ci.extent.height = height;
	image_ci.extent.depth = 1;
	image_ci.arrayLayers = 1;
	image_ci.mipLevels = 1;
	image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
	image_ci.tiling = VK_IMAGE_TILING_LINEAR;
	image_ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo image_alloc_ci = {};
	image_alloc_ci.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	image_alloc_ci.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;

	/* Create the image */
	VkImage dst_image;
	VmaAllocation dst_image_allocation;
	VmaAllocationInfo dst_image_allocation_info;

	if (VK_SUCCESS != vmaCreateImage(dev.get_allocator(), &image_ci, &image_alloc_ci, &dst_image, &dst_image_allocation, &dst_image_allocation_info))
	{
		LOG_FATAL("Failed to create image.");
	}

	/* Do the actual blit from the swapchain image to our host visible destination image */
	VkCommandBuffer cmd_buf = dev.begin_one_time_cmd_buf();

	VkImageSubresourceRange subresource_range = {};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.baseArrayLayer = 0;
	subresource_range.layerCount = 1;

	/* Transition destination image to transfer destination layout */
	vlk_util::insert_image_memory_barrier(
		cmd_buf,
		dst_image,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Transition swapchain image from present to transfer source layout */
	vlk_util::insert_image_memory_barrier(
		cmd_buf,
		src_image,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/*
	If source and destination support blit we'll blit as this also does
	automatic format conversion (e.g. from BGR to RGB)
	*/
	if (supports_blit)
	{
		/* Define the region to blit (we will blit the whole swapchain image) */
		VkOffset3D blit_size;
		blit_size.x = width;
		blit_size.y = height;
		blit_size.z = 1;

		VkImageBlit image_blit_region = {};
		image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit_region.srcSubresource.layerCount = 1;
		image_blit_region.srcOffsets[1] = blit_size;
		image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_blit_region.dstSubresource.layerCount = 1;
		image_blit_region.dstOffsets[1] = blit_size;

		/* Issue the blit command */
		vkCmdBlitImage(
			cmd_buf,
			src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&image_blit_region,
			VK_FILTER_NEAREST);
	}
	else
	{
		/* Otherwise use image copy (requires us to manually flip components) */
		VkImageCopy image_copy_region = {};
		image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_copy_region.srcSubresource.layerCount = 1;
		image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_copy_region.dstSubresource.layerCount = 1;
		image_copy_region.extent.width = width;
		image_copy_region.extent.height = height;
		image_copy_region.extent.depth = 1;

		/* Issue the copy command */
		vkCmdCopyImage(
			cmd_buf,
			src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&image_copy_region);
	}

	/* Transition destination image to general layout, which is the required layout for mapping the image memory later on */
	vlk_util::insert_image_memory_barrier(
		cmd_buf,
		dst_image,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Transition back the swap chain image after the blit is done */
	vlk_util::insert_image_memory_barrier(
		cmd_buf,
		src_image,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		subresource_range);

	/* Submit command buffer */
	dev.end_one_time_cmd_buf(cmd_buf);

	/* Get layout of the image (including row pitch) */
	VkImageSubresource subresource = {};
	subresource.arrayLayer = 0;
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;

	VkSubresourceLayout subresource_layout;
	vkGetImageSubresourceLayout(dev.get_handle(), dst_image, &subresource, &subresource_layout);

	/* Map image memory so we can access it */
	const char* data;
	if (VK_SUCCESS != vkMapMemory(dev.get_handle(), dst_image_allocation_info.deviceMemory, dst_image_allocation_info.offset, dst_image_allocation_info.size, 0, (void**)&data))
	{
		LOG_FATAL("Failed to map Vulkan memory.");
	}

	/*
	If source is BGR (destination is always RGB) and we can't use blit
	(which does automatic conversion), we'll have to manually swizzle color components
	*/
	bool color_swizzle = false;

	/*
	Check if source is BGR
	Note: Not complete, only contains most common and basic BGR surface formats for demonstation purposes
	*/
	if (!supports_blit)
	{
		VkFormat formats_bgr[] =
		{
			VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM
		};

		for (int i = 0; i < cnt_of_array(formats_bgr); ++i)
		{
			if (formats_bgr[i] == src_format)
			{
				color_swizzle = true;
				break;
			}
		}
	}

	/* Get the pixel value for the desired coordinate */
	int pixel_size = 4;
	char* pixel_addr = vlk_util::get_texel_addr(data, subresource_layout, pixel_size, (int)x, (int)y, 0, 0);

	/* Make sure address is valid */
	if (pixel_addr - data > subresource_layout.size)
	{
		LOG_ERROR("Pixel address outside bounds of image buffer.");
		return 0xFFFFFFFF;
	}

	/* Get pixel value as integer */
	int pixel_val = *((int*)pixel_addr);

	/* Deal with BGRA/RGBA if needed */
	if (color_swizzle)
	{
		/* Need to convert from BGRA to RGBA */
		glm::vec4 bgra;
		utl_unpack_rgba_float(pixel_val, bgra);

		/* Swap Blue and Red. Green and Alpha don't change. */
		pixel_val = utl_pack_rgba_float(bgra.z, bgra.y, bgra.x, bgra.w);
	}

	/* Clean up resources */
	vkUnmapMemory(dev.get_handle(), dst_image_allocation_info.deviceMemory);
	vmaDestroyImage(dev.get_allocator(), dst_image, dst_image_allocation);

	return pixel_val;
}


}   /* namespace jetz */
