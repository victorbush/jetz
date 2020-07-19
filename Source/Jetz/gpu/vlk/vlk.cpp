/*=============================================================================
vlk.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/vlk_util.h"
#include "jetz/main/log.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

vlk::vlk()
{
	enable_validation = true;

	create_requirement_lists();
	//create_instance();
	//create_dbg_callbacks();
	//create_device();
}

vlk::~vlk()
{
}

void vlk::wait_idle() const
{
	LOG_FATAL("NOT IMPLEMENTED");
	/* wait for device to finsih current operations. example usage is at
	application exit - wait until current ops finish, then do cleanup. */
	//vkDeviceWaitIdle(TODO);
}

/*=============================================================================
PROTECTED METHODS
=============================================================================*/

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk::create_requirement_lists()
{
	/*-----------------------------------------------------
	Required device extensions
	-----------------------------------------------------*/
	
	/* swap chain support is required */
	required_device_ext.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	/* device extensions aren't validated here since a phyiscal device is required */

	/*-----------------------------------------------------
	Required instance layers
	-----------------------------------------------------*/

	/* add validation layers */
	if (enable_validation)
	{
		required_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	if (!vlk_util::are_instance_layers_available(required_instance_layers))
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
		required_instance_ext.push_back(glfw_extensions[i]);
	}

	/* determine other extensions needed */
	if (enable_validation)
	{
		/* debug report extension is required to use validation layers */
		required_instance_ext.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	/* throw error if the desired extensions are not available */
	if (!vlk_util::are_instance_extensions_available(required_instance_ext))
	{
		LOG_FATAL("Required instance extensions are not available.");
	}
}

}   /* namespace jetz */
