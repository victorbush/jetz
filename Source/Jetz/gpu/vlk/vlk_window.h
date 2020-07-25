/*=============================================================================
vlk_window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_swapchain.h"
#include "jetz/main/camera.h"
#include "jetz/main/window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

/**
Vulkan logical device.
*/
class vlk_window : gpu_window {

public:

	vlk_window(VkSurfaceKHR surface, window& app_window);
	~vlk_window();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	void begin_frame(vlk_frame& frame, const camera& camera);
	void end_frame(const vlk_frame& frame);
	void resize(uint32_t width, uint32_t height);

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_descriptors();
	void create_pipelines();
	void create_swapchain(uint32_t width, uint32_t height);

	void destory_descriptors();
	void destory_pipelines();
	void destory_surface();
	void destory_swapchain();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	window&				app_window;
	VkSurfaceKHR		surface;		/* Although this is a dependency, we must destroy it ourselves */

	/*
	Create/destroy
	*/
	vlk_swapchain*		swapchain;
};

}   /* namespace jetz */
