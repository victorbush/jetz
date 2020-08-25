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
#include "jetz/main/window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
class vlk_window;

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

	/* The maximum number of materials allowed. */
	static int max_num_materials;

	/**
	The number of frame buffers to use for rendering.
	Double buffered == 2
	Triple buffered == 3
	*/
	static int num_frame_buf;

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	VkInstance get_instance() const;
	vlk_device& get_device() const;

	/*-----------------------------------------------------
	jetz::gpu methods
	-----------------------------------------------------*/

	virtual void wait_idle() const override;

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
	window&							_app_window;

	/*
	Create/destroy
	*/
	vlk_device*						_dev;					/* logical device */
	VkDebugReportCallbackEXT		_dbg_callback_hndl;
	vlk_gpu*						_gpu;					/* physical device */
	VkInstance						_instance;
	VkSurfaceKHR					_surface;
	vlk_window*						_vlk_window;

	std::vector<const char*>		_required_device_ext;		/* required device extensions */
	std::vector<const char*>		_required_instance_ext;		/* required instance extensions */
	std::vector<const char*>		_required_instance_layers;	/* required instance layers */

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
