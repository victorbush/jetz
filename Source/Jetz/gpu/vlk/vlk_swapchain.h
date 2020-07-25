/*=============================================================================
vlk_swapchain.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_frame.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_swapchain {

public:

	vlk_swapchain
		(
		vlk_device&					dev,			/* Logical device the surface is tied to */
		VkSurfaceKHR				surface,		/* Surface to use */
		VkExtent2D					extent			/* Desired swapchain extent */
		);

	~vlk_swapchain();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Begins the next frame.
	*/
	void begin_frame(vlk_frame& frame);

	/**
	Ends the specified frame.
	*/
	void end_frame(const vlk_frame& frame);

	/**
	Gets the command buffer for the specified frame.
	*/
	VkCommandBuffer get_cmd_buf(const vlk_frame& frame) const;

	/**
	Gets the swapchain extent.
	*/
	VkExtent2D get_extent() const;

	/**
	Flags swap chain to resize itself.
	*/
	void recreate(int width, int height);

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void begin_picker_render_pass(vlk_frame& frame);
	void begin_primary_render_pass(vlk_frame& frame);
	
	/**
	Chooses best swap extent based on surface capabilties.
	*/
	VkExtent2D choose_swap_extent(VkExtent2D desired_extent, VkSurfaceCapabilitiesKHR* capabilities) const;

	/**
	Creates the swap chain and all associated resources.
	*/
	void create_all(VkExtent2D extent);
	void create_command_buffers();
	void create_depth_buffer();
	void create_framebuffers();
	void create_image_views();
	void create_semaphores();
	void create_swapchain(VkExtent2D extent);

	/**
	Destroys the swap chain and all associated resources.
	*/
	void destroy_all();
	void destroy_command_buffers();
	void destroy_depth_buffer();
	void destroy_framebuffers();
	void destroy_image_views();
	void destroy_semaphores();
	void destroy_swapchain();

	/**
	Creates images and image views for the picker buffers. Objects are rendered
	to the a picker buffer using a unique color. The buffer can then be sampled at a certain
	pixel to determine what object is drawn at that pixel. It allows "picking" objects on the screen.
	*/
	void create_picker_buffers();

	/**
	Destroys the picker buffer images and image views.
	*/
	void destroy_picker_buffers();

	/**
	Recreates the swap chain and associated resources.
	*/
	void resize(VkExtent2D extent);

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&						dev;					/* logical device */
	vlk_gpu&						gpu;					/* phsyical device */
	VkSurfaceKHR					surface;				/* surface handle */

	/*
	Create/destroy
	*/
	std::vector<VkCommandBuffer>	cmd_bufs;				/* implicitly destroy by command pools */
	std::vector<VkImage>			depth_images;
	std::vector<VkImageView>		depth_image_views;
	std::vector<VmaAllocation>		depth_image_allocations;
	std::vector<VkFramebuffer>		frame_bufs;				/* swapchain framebuffers */
	VkSwapchainKHR					handle;					/* swapchain handle */
	std::vector<VkImage>			images;					/* swapchain images */
	std::vector<VkImageView>		image_views;			/* swapchain image views */

	std::vector<VkCommandBuffer>	picker_cmd_bufs;		/* implicitly destroy by command pools */
	std::vector<VkFramebuffer>		picker_frame_bufs;		/* picker render pass framebuffers */
	std::vector<VkImage>			picker_images;
	std::vector<VmaAllocation>		picker_image_allocations;
	std::vector<VkImageView>		picker_image_views;

	std::vector<VkFence>			in_flight_fences;
	std::vector<VkSemaphore>		image_avail_semaphores;
	std::vector<VkSemaphore>		render_finished_semaphores;

	/*
	Swapchain properties
	*/
	VkExtent2D						extent;
	VkPresentModeKHR				present_mode;
	VkSurfaceFormatKHR				surface_format;

	/*
	Other
	*/
	double							last_time;				/* time the previous frame was started */
};

}   /* namespace jetz */
