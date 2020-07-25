/*=============================================================================
vlk_window.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_window.h"
#include "jetz/gpu/vlk/vlk_window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_window::vlk_window(VkSurfaceKHR surface, window& app_window)
	: app_window(app_window),
	surface(surface)
{
	create_swapchain(app_window.get_width(), app_window.get_height());
	create_pipelines();
	create_descriptors();

	app_window.set_gpu_window((gpu_window*)this);
}

vlk_window::~vlk_window()
{
	destroy_descriptors();
	destroy_pipelines();
	destroy_swapchain();
	destroy_surface();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_window::begin_frame(vlk_frame& frame, const camera& camera)
{
	/* Setup render pass, command buffer, etc. */
	swapchain->begin_frame(frame);

	/* Setup per-view descriptor set data */
	_vlk_per_view_set__update(&vlk_window->per_view_set, vlk_frame, camera, vlk_window->swapchain.extent);

	
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
