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
	
class gpu_factory;
class vlk_factory;

/*=============================================================================
CLASS
=============================================================================*/

class vlk : public gpu {

public:

	vlk(window& app_window);
	~vlk();

	/*-----------------------------------------------------
	Public static variables
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	VkInstance get_instance() const;
	vlk_device& get_device() const;

	/*-----------------------------------------------------
	jetz::gpu methods
	-----------------------------------------------------*/

	virtual sptr<gpu_factory> get_factory() const override;
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
	sptr<vlk_factory>				_factory;
	vlk_gpu*						_gpu;					/* physical device */
	VkInstance						_instance;
	VkSurfaceKHR					_surface;

	std::vector<const char*>		_required_device_ext;		/* required device extensions */
	std::vector<const char*>		_required_instance_ext;		/* required instance extensions */
	std::vector<const char*>		_required_instance_layers;	/* required instance layers */

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/
	
	void create_device();
	void create_dbg_callbacks();
	void create_factory();
	void create_instance();
	void create_requirement_lists();

	void destroy_device();
	void destroy_dbg_callbacks();
	void destroy_factory();
	void destroy_instance();
	void destroy_requirement_lists();

	void select_physical_device();
};

}   /* namespace jetz */
