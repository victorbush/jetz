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

public:

	vlk_gpu
		(
		const VkPhysicalDevice		physical_device,	/* Physical device handle for this GPU  */
		const VkSurfaceKHR			surface				/* Used to query for surface capabilties */
		);

	~vlk_gpu();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	Finds index of memory type with the desired properties.
	*/
	uint32_t find_memory_type_idx
		(
		uint32_t					type_filter, 
		VkMemoryPropertyFlags		properties
		) const;

	/**
	Finds a supported surface format in a list of candidates for this device.
	*/
	VkFormat find_supported_format
		(
		const std::vector<VkFormat>&	candidates,
		VkImageTiling					tiling,
		VkFormatFeatureFlags			features
		) const;

	/**
	Gets the depth buffer format.
	*/
	VkFormat get_depth_format() const;

	/**
	Gets the Vulkan physical device handle.
	*/
	VkPhysicalDevice get_handle() const;

	/**
	Query device for current surface capabilities. Some of the capabilties don't
	change, but the surface extent does. This is just a wrapper around the 
	Vulkan API call.
	*/
	VkResult query_surface_capabilties
		(
		const VkSurfaceKHR				surface,			/* The surface to get capabilties for */
		VkSurfaceCapabilitiesKHR&		capabilties			/* Output - the surface capabilties */
		) const;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/
	
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
	VkPhysicalDeviceFeatures		supported_features;			/* Features supported by this device */
	VkPresentModeKHR				optimal_present_mode;		/* Preferred presentation mode based of available modes for this GPU */
	VkSurfaceFormatKHR				optimal_surface_format;		/* The preferred surface we chose based on available formats for this GPU */

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	VkPhysicalDevice				handle;						/* Physical device handle - automatically destroyed by Vulkan instance */

	/*
	Other
	*/
	VkPhysicalDeviceProperties		device_properties;
	VkPhysicalDeviceMemoryProperties
									mem_properties;
};

}   /* namespace jetz */
