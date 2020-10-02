/*=============================================================================
vlk_window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_swapchain.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_set.h"
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
class vlk_window : public gpu_window {

public:

	vlk_window(vlk_device& device, VkInstance vkInstance, VkSurfaceKHR surface, uint32_t width, uint32_t height);
	~vlk_window();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	vlk_frame& get_frame(const gpu_frame& gpu_frame);
	int get_picker_id(const vlk_frame& frame, float x, float y);

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/
	
	virtual gpu_frame& do_begin_frame(camera& cam) override;
	virtual void do_end_frame(const gpu_frame& frame) override;
	virtual void do_render_imgui(const gpu_frame& frame, ImDrawData* draw_data) override;
	virtual void do_resize() override;

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_descriptors();
	void create_frame_info();
	void create_swapchain(uint32_t width, uint32_t height);

	void destroy_descriptors();
	void destroy_frame_info();
	void destroy_surface();
	void destroy_swapchain();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					dev;
	VkInstance					instance;
	VkSurfaceKHR				surface;		/* Although this is a dependency, we must destroy it ourselves */

	/*
	Create/destroy
	*/
	vlk_per_view_set*			per_view_set;
	vlk_swapchain*				swapchain;

	/*
	Create/destroy
	*/
	std::vector<vlk_frame>		_frames;
};

}   /* namespace jetz */
