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
#include "jetz/gpu/vlk/descriptors/vlk_per_view_set.h"
#include "jetz/gpu/vlk/pipelines/vlk_imgui_pipeline.h"
#include "jetz/main/camera.h"
#include "thirdparty/imgui/imgui.h"

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

	vlk_window(vlk_device& device, VkInstance vkInstance, VkSurfaceKHR surface, uint32_t width, uint32_t height);
	~vlk_window();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	void begin_frame(vlk_frame& frame, const camera& camera);
	void end_frame(const vlk_frame& frame);
	int get_picker_id(const vlk_frame& frame, float x, float y);
	void render_imgui(vlk_frame& frame, ImDrawData* draw_data);
	void resize(uint32_t width, uint32_t height);

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_descriptors();
	void create_pipelines();
	void create_swapchain(uint32_t width, uint32_t height);

	void destroy_descriptors();
	void destroy_pipelines();
	void destroy_surface();
	void destroy_swapchain();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&			dev;
	VkInstance			instance;
	VkSurfaceKHR		surface;		/* Although this is a dependency, we must destroy it ourselves */

	/*
	Create/destroy
	*/
	vlk_per_view_set*	per_view_set;
	vlk_swapchain*		swapchain;

	vlk_imgui_pipeline*	imgui_pipeline;
};

}   /* namespace jetz */
