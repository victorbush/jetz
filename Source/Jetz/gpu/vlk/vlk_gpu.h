/*=============================================================================
vlk_gpu.h
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
TYPES
=============================================================================*/

/**
Queue family indices.
*/
struct vlk_gpu_qfi
{
	std::vector<uint32_t>			graphics_families;
	std::vector<uint32_t>			present_families;
};

/**
Vulkan physical device.
*/
class vlk_gpu {

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/* Physical device handle - automatically destroyed by Vulkan instance */
	VkPhysicalDevice                handle;

	/*
	Create/destroy
	*/
	std::vector<VkPresentModeKHR>	avail_present_modes;		/* Available present modes */
	std::vector<VkSurfaceFormatKHR>	avail_surface_formats;		/* Available surface formats */
	std::vector<VkExtensionProperties>
									ext_properties;				/* Device extension info */
	std::vector<VkQueueFamilyProperties>
									queue_family_props;			/* Queue family properties */
	vlk_gpu_qfi						queue_family_indices;		/* Stores indices to certain types of queue families */

	/*
	Other
	*/
	VkPhysicalDeviceProperties		device_properties;
	VkPhysicalDeviceMemoryProperties
									mem_properties;
	VkPresentModeKHR				optimal_present_mode;		/* Preferred presentation mode based of available modes for this GPU */
	VkSurfaceFormatKHR				optimal_surface_format;		/* The preferred surface we chose based on available formats for this GPU */
	VkPhysicalDeviceFeatures		supported_features;			/* Features supported by this device */
};

}   /* namespace jetz */
