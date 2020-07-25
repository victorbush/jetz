/*=============================================================================
vlk.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/gpu.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_gpu.h"
#include "jetz/gpu/vlk/vlk_window.h"
#include "jetz/main/window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk : gpu {

public:

	vlk(window& app_window);
	~vlk();

	/*-----------------------------------------------------
	Public static variables
	-----------------------------------------------------*/

	/**
	The number of frame buffers to use for rendering.
	Double buffered == 2
	Triple buffered == 3
	*/
	static int num_frame_buf;

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	jetz::gpu::wait_idle
	*/
	virtual void wait_idle() const;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

	/** Enable Vulkan validation layers? */
	bool enable_validation;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	GLFWwindow*						glfw_window;
	window&							app_window;

	/*
	Create/destroy
	*/
	vlk_device*						dev;					/* logical device */
	VkDebugReportCallbackEXT		dbg_callback_hndl;
	vlk_gpu*						gpu;					/* physical device */
	VkInstance						instance;
	VkSurfaceKHR					surface;
	vlk_window*						vlk_window;

	std::vector<const char*>		required_device_ext;		/* required device extensions */
	std::vector<const char*>		required_instance_ext;		/* required instance extensions */
	std::vector<const char*>		required_instance_layers;	/* required instance layers */

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/
	
	void create_device();
	void create_dbg_callbacks();
	void create_instance();
	void create_requirement_lists();

	void destroy_device();
	void destroy_dbg_callbacks();
	void destroy_instance();
	void destroy_requirement_lists();

	void select_physical_device();
};

}   /* namespace jetz */
