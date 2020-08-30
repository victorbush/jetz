/*=============================================================================
vlk_gpu.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_gpu.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
STATIC FUNCTIONS
=============================================================================*/

static VkPresentModeKHR choose_present_mode
	(
	const std::vector<VkPresentModeKHR>&	avail_modes		/* modes to choose from         */
	)
{
	uint32_t i;

	/* desired mode is mailbox mode (allows triple buffering) */
	const VkPresentModeKHR desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;

	/* see if desired mode is available */
	for (i = 0; i < avail_modes.size(); ++i)
	{
		VkPresentModeKHR mode = avail_modes[i];

		if (mode == desiredMode)
		{
			return desiredMode;
		}
	}

	/* default to FIFO since always available */
	return VK_PRESENT_MODE_FIFO_KHR;
}

static VkSurfaceFormatKHR choose_surface_format
	(
	const std::vector<VkSurfaceFormatKHR>&	avail_formats		/* formats to choose from       */
	)
{
	uint32_t i;
	VkSurfaceFormatKHR result;

	/* if undefined format, then any format can be chosen */
	if (avail_formats.size() == 1 
		&& avail_formats[0].format == VK_FORMAT_UNDEFINED)
	{
		result.format = VK_FORMAT_B8G8R8A8_UNORM;
		result.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		return (result);
	}

	/* if formats are restricted, look for the preferred format */
	for (i = 0; i < avail_formats.size(); ++i)
	{
		VkSurfaceFormatKHR format = avail_formats[i];

		if (format.format == VK_FORMAT_B8G8R8A8_UNORM
			&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	/* if desired format not found, just use the first available format */
	return avail_formats[0];
}

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_gpu::vlk_gpu
	(
	const VkPhysicalDevice			physical_device,	/* Physical device handle for this GPU  */
	const VkSurfaceKHR				surface				/* Used to query for surface capabilties */
	)
{
	handle = physical_device;
	
	/*-----------------------------------------------------

	Device extensions

	-----------------------------------------------------*/

	/* get number of extensions */
	uint32_t num_ext = 0;
	vkEnumerateDeviceExtensionProperties(handle, NULL, &num_ext, NULL);

	/* get list of extensions */
	ext_properties.resize(num_ext);
	vkEnumerateDeviceExtensionProperties(handle, NULL, &num_ext, ext_properties.data());

	/*-----------------------------------------------------

	Device features

	-----------------------------------------------------*/

	vkGetPhysicalDeviceFeatures(handle, &supported_features);

	/*-----------------------------------------------------

	Device properties

	-----------------------------------------------------*/

	vkGetPhysicalDeviceProperties(handle, &device_properties);

	/*-----------------------------------------------------

	Surface related attributes

	-----------------------------------------------------*/

	/*
	Surface formats
	*/
	uint32_t num_format;
	vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &num_format, NULL);

	if (num_format != 0)
	{
		avail_surface_formats.resize(num_format);
		vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &num_format, avail_surface_formats.data());
	}

	/*
	Presentation modes
	*/
	uint32_t num_present_mode;
	vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &num_present_mode, NULL);

	if (num_present_mode != 0)
	{
		avail_present_modes.resize(num_present_mode);
		vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &num_present_mode, avail_present_modes.data());
	}

	/*
	* Based on device capabilities, choose the ideal formats to use
	*/
	optimal_surface_format = choose_surface_format(avail_surface_formats);
	optimal_present_mode = choose_present_mode(avail_present_modes);

	/*-----------------------------------------------------

	Queue families

	-----------------------------------------------------*/

	/* get number of queue families */
	uint32_t num_queue_fam = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(handle, &num_queue_fam, NULL);

	/* get list of queue families */
	queue_family_props.resize(num_queue_fam);
	vkGetPhysicalDeviceQueueFamilyProperties(handle, &num_queue_fam, queue_family_props.data());

	/* determine what things each family supports */
	for (uint32_t i = 0; i < queue_family_props.size(); ++i)
	{
		const VkQueueFamilyProperties* family = &queue_family_props[i];

		/* check # queue */
		if (family->queueCount == 0)
		{
			/* no queues (??) */
			continue;
		}

		/* check for presentation support */
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, &presentationSupport);

		if (presentationSupport)
		{
			queue_family_indices.present_families.push_back(i);
		}

		/* check for graphics support */
		if (family->queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			queue_family_indices.graphics_families.push_back(i);
		}
	}

	/*-----------------------------------------------------

	Memory properties

	-----------------------------------------------------*/

	vkGetPhysicalDeviceMemoryProperties(handle, &mem_properties);
}

vlk_gpu::~vlk_gpu()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

uint32_t vlk_gpu::find_memory_type_idx
	(
	uint32_t				type_filter, 
	VkMemoryPropertyFlags	properties
	) const
{
	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
	{
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	LOG_FATAL("Failed to find suitable memory type.");
	return 0;
}

VkFormat vlk_gpu::find_supported_format
	(
	const std::vector<VkFormat>&	candidates,
	VkImageTiling					tiling,
	VkFormatFeatureFlags			features
	) const
{
	for (uint32_t i = 0; i < candidates.size(); ++i)
	{
		VkFormat format = candidates[i];
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(handle, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR
			&& (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL
			&& (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	LOG_FATAL("Failed to find supported format.");
	return VK_FORMAT_R32G32B32A32_UINT;
}

VkFormat vlk_gpu::get_depth_format() const
{
	std::vector<VkFormat> candidates
	{
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT
	};

	VkFormat format = find_supported_format(
		candidates,
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	return format;
}

VkPhysicalDevice vlk_gpu::get_handle() const
{
	return handle;
}

VkResult vlk_gpu::query_surface_capabilties
	(
	const VkSurfaceKHR			surface,
	VkSurfaceCapabilitiesKHR&	capabilties
	) const
{
	return vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, &capabilties);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
