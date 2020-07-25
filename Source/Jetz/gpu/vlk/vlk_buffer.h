/*=============================================================================
vlk_buffer.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"
#include "thirdparty/vma/vma.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

class vlk_buffer {

public:

	vlk_buffer
		(
		vlk_device&					device,
		VkDeviceSize				size,
		VkBufferUsageFlags			buffer_usage,
		VmaMemoryUsage				memory_usage
		);

	~vlk_buffer();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	Builds a VkDescriptorBufferInfo struct for this buffer.
	*/
	VkDescriptorBufferInfo get_buffer_info() const;

	/**
	Updates the data in the buffer.
	*/
	void update(void* data, VkDeviceSize offset, VkDeviceSize size);

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void update_direct
		(
		void*					data,
		VkDeviceSize			offset,
		VkDeviceSize			data_size
		);

	void update_via_staging_buffer
		(
		void*					data, 
		VkDeviceSize			offset, 
		VkDeviceSize			data_size
		);

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	VmaAllocation					allocation;
	VkBufferUsageFlags				buffer_usage;	/* how the buffer is used */
	vlk_device&						dev;			/* logical device */
	VkBuffer						handle;			/* Vulkan buffer handle */
	VmaMemoryUsage					memory_usage;	/* how the underlying memory is used */
	VkDeviceSize					size;			/* the size of the buffer */
};

}   /* namespace jetz */
