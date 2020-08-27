/*=============================================================================
vlk.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/vlk_window.h"
#include "jetz/gpu/vlk/vlk_util.h"
#include "jetz/main/window.h"
#include "jetz/main/log.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/* Default to double buffering. */
int vlk::num_frame_buf = 2;

int vlk::max_num_materials = 100;

/*=============================================================================
STATIC FUNCTIONS
=============================================================================*/

/**
Vulkan debug callback.
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback
	(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char *layerPrefix,
	const char *msg,
	void *userData
	)
{
	LOG_ERROR(msg);
	return VK_FALSE;
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

vlk::vlk(window& app_window) : _app_window(app_window)
{
	enable_validation = true;

	create_requirement_lists();
	create_instance();
	create_dbg_callbacks();
	create_device();
}

vlk::~vlk()
{
	destroy_device();
	destroy_dbg_callbacks();
	destroy_instance();
	destroy_requirement_lists();
}

vlk_device& vlk::get_device() const
{
	return *_dev;
}

void vlk::wait_idle() const
{
	/* wait for device to finsih current operations. example usage is at
	application exit - wait until current ops finish, then do cleanup. */
	vkDeviceWaitIdle(_dev->get_handle());
}

/*=============================================================================
PROTECTED METHODS
=============================================================================*/

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk::create_device()
{
	/* Create a surface - destroyed by vlk_window */
	if (glfwCreateWindowSurface(_instance, _app_window.get_hndl(), NULL, &_surface) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create window surface.");
	}

	/* Select a physical device - the surface we created helps determine what device to use */
	select_physical_device();

	/* Create logical device */
	_dev = new jetz::vlk_device(*_gpu, _required_device_ext, _required_instance_layers);

	/* Must check to make sure surface supports presentation */
	VkBool32 supported = 0;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(_gpu->get_handle(), _dev->get_present_family_idx(), _surface, &supported);
	if (result != VK_SUCCESS || !supported)
	{
		LOG_FATAL("Surface not supported for this GPU.");
	}

	/* Create gpu window */
	_vlk_window = new jetz::vlk_window(*_dev, _instance, _surface, _app_window.get_width(), _app_window.get_height());
	_app_window.set_gpu_window((gpu_window*)_vlk_window);
}

void vlk::create_dbg_callbacks()
{
	/* only do this if validation layers are enabled*/
	if (!enable_validation)
	{
		return;
	}

	/* setup the create info */
	VkDebugReportCallbackCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	create_info.pfnCallback = debug_callback;

	/*
	Get address to the create callback function. In order for this to work,
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME must be added to the list of required
	extensions when the Vulkan instance is created.
	*/
	PFN_vkCreateDebugReportCallbackEXT func;
	func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
	VkResult result = VK_SUCCESS;

	/* create the callback */
	if (func != NULL)
	{
		result = func(_instance, &create_info, NULL, &_dbg_callback_hndl);
	}

	if (func == NULL || result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create Vulkan debug callback.");
	}
}

void vlk::create_instance()
{
	// Can be used if Steam validation layers cause issues
	//_putenv("DISABLE_VK_LAYER_VALVE_steam_overlay_1=1");

	/*
	Build instance creation data
	*/

	/* build app info */
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Jetz";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Jetz Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	/* build create info */
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	/* get required Vulkan extensions */
	create_info.enabledExtensionCount = (uint32_t)_required_instance_ext.size();
	create_info.ppEnabledExtensionNames = _required_instance_ext.data();

	/* get validation layers */
	create_info.enabledLayerCount = (uint32_t)_required_instance_layers.size();
	create_info.ppEnabledLayerNames = _required_instance_layers.data();

	/*
	* Create the instance
	*/
	VkResult result = vkCreateInstance(&create_info, NULL, &_instance);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create Vulkan instance.");
	}
}

void vlk::create_requirement_lists()
{
	/*-----------------------------------------------------
	Required device extensions
	-----------------------------------------------------*/
	
	/* swap chain support is required */
	_required_device_ext.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/* device extensions aren't validated here since a phyiscal device is required */

	/*-----------------------------------------------------
	Required instance layers
	-----------------------------------------------------*/

	/* add validation layers */
	if (enable_validation)
	{
		_required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	if (!vlk_util::are_instance_layers_available(_required_instance_layers))
	{
		LOG_FATAL("Required instance layers are not available.");
	}

	/*-----------------------------------------------------
	Required instance extensions
	-----------------------------------------------------*/

	/* get extensions required by GLFW */
	uint32_t num_glfw_extensions = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);

	/* add glfw extensions */
	for (uint32_t i = 0; i < num_glfw_extensions; ++i)
	{
		_required_instance_ext.push_back(glfw_extensions[i]);
	}

	/* determine other extensions needed */
	if (enable_validation)
	{
		/* debug report extension is required to use validation layers */
		_required_instance_ext.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	/* throw error if the desired extensions are not available */
	if (!vlk_util::are_instance_extensions_available(_required_instance_ext))
	{
		LOG_FATAL("Required instance extensions are not available.");
	}
}

void vlk::destroy_device()
{
	delete _vlk_window;
	delete _dev;
	delete _gpu;
}

void vlk::destroy_dbg_callbacks()
{
	/* only do this if validation layers are enabled*/
	if (!enable_validation)
	{
		return;
	}

	/* get address to the create callback function*/
	PFN_vkDestroyDebugReportCallbackEXT func;
	func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");

	/* destroy the callback */
	if (func != NULL)
	{
		func(_instance, _dbg_callback_hndl, NULL);
	}
	else
	{
		LOG_FATAL("Failed to destroy Vulkan debug callback.");
	}
}

void vlk::destroy_instance()
{
	vkDestroyInstance(_instance, NULL);
}

void vlk::destroy_requirement_lists()
{
	/* Nothing to do */
}

void vlk::select_physical_device()
{
	std::vector<VkPhysicalDevice>	devices;
	std::vector<vlk_gpu*>			gpus;
	std::vector<const char*>		device_ext;		/* required device extensions */

	/* check assumptions */
	if (_instance == VK_NULL_HANDLE)
	{
		LOG_FATAL("Vulkan instance must be created before selecting a phyiscal device.");
	}

	if (_surface == VK_NULL_HANDLE)
	{
		LOG_FATAL("Vulkan surface must be created before selecting a phyiscal device.");
	}

	/*
	Device extensions
	*/

	/* swap chain support is required */
	device_ext.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/*
	Enumerate physical devices
	*/

	/* get number of GPUs */
	uint32_t num_gpus = 0;
	vkEnumeratePhysicalDevices(_instance, &num_gpus, NULL);
	if (num_gpus == 0)
	{
		LOG_FATAL("No GPU found with Vulkan support.");
	}

	/* get the list of devices */
	devices.resize(num_gpus);
	vkEnumeratePhysicalDevices(_instance, &num_gpus, devices.data());

	/* allocate mem for GPU info */
	gpus.resize(num_gpus);

	/* get info for each physical device */
	for (uint32_t i = 0; i < num_gpus; i++)
	{
		gpus[i] = new vlk_gpu(devices[i], _surface);
	}

	/*
	Select a physical device
	*/
	
	/* inspect the physical devices and select one */
	for (uint32_t i = 0; i < num_gpus; ++i)
	{
		vlk_gpu* gpu = gpus[i];
		
		/*
		* Check required extensions
		*/
		if (!vlk_util::are_device_extensions_available(device_ext, *gpu))
		{
			continue;
		}

		/*
		* Check surface formats
		*/
		if (gpu->avail_surface_formats.size() == 0)
		{
			/* no surface formats */
			continue;
		}

		/*
		* Check presentation modes
		*/
		if (gpu->avail_present_modes.size() == 0)
		{
			/* no presentation modes */
			continue;
		}

		/*
		* Check queue families
		*/
		vlk_gpu_qfi* qfi = &gpu->queue_family_indices;

		/* graphics */
		if (qfi->graphics_families.size() == 0)
		{
			/* no graphics queue families */
			continue;
		}

		/* presentation */
		if (qfi->present_families.size() == 0)
		{
			/* no present queue families */
			continue;
		}

		/*
		* This GPU meets our requirements, use it
		*/

		/* init new gpu info for selected GPU and destroy the temp array */
		_gpu = new jetz::vlk_gpu(gpu->get_handle(), _surface);
		break;
	}

	/* verify a physical device was selected */
	if (_gpu == nullptr) 
	{
		LOG_FATAL("Failed to find a suitable GPU.");
	}

	/* Cleanup temp GPU info array */
	for (uint32_t i = 0; i < num_gpus; i++)
	{
		delete gpus[i];
	}

	gpus.clear();
}

}   /* namespace jetz */
