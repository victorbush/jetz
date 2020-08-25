/*=============================================================================
vlk_buffer.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_buffer::vlk_buffer
	(
	vlk_device&				device, 
	VkDeviceSize			size, 
	VkBufferUsageFlags		buffer_usage, 
	VmaMemoryUsage			memory_usage
	)
	: dev(device),
	size(size),
	buffer_usage(buffer_usage),
	memory_usage(memory_usage)
{
	if (memory_usage & VMA_MEMORY_USAGE_GPU_ONLY)
	{
		/* Force transfer destination since will be using with staging buffer */
		buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	VkBufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.size = size;
	info.usage = buffer_usage;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = memory_usage;

	VkResult result = vmaCreateBuffer(dev.get_allocator(), &info, &alloc_info, &handle, &allocation, NULL);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create buffer.");
	}
}

vlk_buffer::~vlk_buffer()
{
	vmaDestroyBuffer(dev.get_allocator(), handle, allocation);
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

VmaAllocation vlk_buffer::get_allocation() const
{
	return allocation;
}

VkBuffer vlk_buffer::get_handle() const
{
	return handle;
}

VkDescriptorBufferInfo vlk_buffer::get_buffer_info() const
{
	VkDescriptorBufferInfo info = {};
	info.buffer = handle;
	info.offset = 0;
	info.range = size;
	return info;
}

void vlk_buffer::update(void* data, VkDeviceSize offset, VkDeviceSize size)
{
	switch (memory_usage)
	{
	case VMA_MEMORY_USAGE_GPU_ONLY:
		update_via_staging_buffer(data, offset, size);
		break;

	default:
		update_direct(data, offset, size);
		break;
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_buffer::update_direct(void* data, VkDeviceSize offset, VkDeviceSize data_size)
{
	void* buf;
	VkResult result = vmaMapMemory(dev.get_allocator(), allocation, &buf);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Unable to map Vulkan memory.");
	}

	buf = (char*)buf + offset;
	memcpy(buf, data, data_size);
	vmaUnmapMemory(dev.get_allocator(), allocation);
}

void vlk_buffer::update_via_staging_buffer(void* data, VkDeviceSize offset, VkDeviceSize data_size)
{
	/*
	Create staging buffer
	*/
	vlk_buffer staging_buffer = vlk_buffer(
		dev,
		data_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_ONLY);

	/*
	Load data to staging buffer
	*/
	staging_buffer.update(data, 0, data_size);

	/*
	Copy staging -> GPU
	*/
	VkCommandBuffer cmd_buf = dev.begin_one_time_cmd_buf();

	VkBufferCopy copy_region = {};
	copy_region.size = data_size;
	copy_region.dstOffset = offset;
	copy_region.srcOffset = 0;
	vkCmdCopyBuffer(cmd_buf, staging_buffer.handle, handle, 1, &copy_region);

	dev.end_one_time_cmd_buf(cmd_buf);

	/*
	Free staging buffer - done in destructor
	*/
}

}   /* namespace jetz */
