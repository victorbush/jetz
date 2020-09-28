/*=============================================================================
vlk_texture.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/gpu/vlk/vlk_texture.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_texture::vlk_texture
	(
	vlk_device&						device,
	const vlk_texture_create_info&	create_info
	)
	: dev(device)
{
	create_image(create_info);
	create_image_view();
	init_image_info();
}

vlk_texture::~vlk_texture()
{
	destroy_image_view();
	destroy_image();
}

VkImage vlk_texture::get_image() const
{
	return image;
}

VkDescriptorImageInfo* vlk_texture::get_image_info() const
{
	return &image_info;
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_texture::create_image(const vlk_texture_create_info& create_info)
{
	/*
	Create staging buffer with texture data
	*/
	vlk_buffer staging_buffer(
		dev,
		create_info.size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	staging_buffer.update(create_info.data, 0, create_info.size);

	/*
	Create the image
	*/
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = create_info.width;
	image_info.extent.height = create_info.height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;

	// TODO : assuming the format for now
	format = VK_FORMAT_R8G8B8A8_UNORM;

	image_info.format = format;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	VkResult result = vmaCreateImage(dev.get_allocator(), &image_info, &alloc_info, &image, &image_allocation, NULL);
	if (result != VK_SUCCESS) 
	{
		LOG_FATAL("Failed to create texture image.");
	}

	/*
	Copy texture from staging buffer to GPU
	*/
	dev.transition_image_layout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	dev.copy_buffer_to_img_now(staging_buffer.get_handle(), image, create_info.width, create_info.height);
	dev.transition_image_layout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	/*
	Cleanup staging buffer - desctructor called for stack variable
	*/
}

void vlk_texture::create_image_view()
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	if (vkCreateImageView(dev.get_handle(), &view_info, NULL, &image_view) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create texture image view.");
	}
}

void vlk_texture::init_image_info()
{
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = image_view;
	image_info.sampler = dev.get_texture_sampler();
}

void vlk_texture::destroy_image()
{
	vmaDestroyImage(dev.get_allocator(), image, image_allocation);
}

void vlk_texture::destroy_image_view()
{
	vkDestroyImageView(dev.get_handle(), image_view, NULL);
}

}   /* namespace jetz */
