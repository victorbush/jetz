/*=============================================================================
vlk_swapchain.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"
#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/vlk_swapchain.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_swapchain::vlk_swapchain
	(
	vlk_device&					dev,			/* Logical device the surface is tied to */
	VkSurfaceKHR				surface,		/* Surface to use */
	VkExtent2D					extent			/* Desired swapchain extent */
	)
	: dev(dev),
	gpu(dev.get_gpu()),
	surface(surface),
	_extent(extent)
{
	/* create everything */
	create_all(extent);

	last_time = glfwGetTime();
}

vlk_swapchain::~vlk_swapchain()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_swapchain::begin_frame(vlk_frame& frame)
{
	vlk_frame_status frame_status = vlk_frame_status::INVALID;

	vkWaitForFences(dev.get_handle(), 1, &in_flight_fences[frame.frame_idx], VK_TRUE, UINT64_MAX);
	vkResetFences(dev.get_handle(), 1, &in_flight_fences[frame.frame_idx]);

	VkResult result = vkAcquireNextImageKHR(dev.get_handle(), handle, UINT64_MAX, image_avail_semaphores[frame.frame_idx], VK_NULL_HANDLE, &frame.image_idx);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize(_extent);
		frame_status = vlk_frame_status::SWAPCHAIN_OUT_OF_DATE;
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LOG_FATAL("Failed to acquire swapchain image.");
	}

	/* Start render passes */
	begin_primary_render_pass(frame);
	begin_picker_render_pass(frame);

	/* Calc frame timing */
	double curTime = glfwGetTime();
	frame.delta_time = curTime - last_time;
	last_time = curTime;
}

void vlk_swapchain::end_frame(const vlk_frame& frame)
{
	uint32_t img_idx = frame.image_idx;
	VkCommandBuffer cmd = cmd_bufs[img_idx];

	/* End the primary render pass */
	vkCmdEndRenderPass(cmd);

	VkResult result = vkEndCommandBuffer(cmd);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to record command buffer.");
	}

	/* End the picker buffer render pass */
	vkCmdEndRenderPass(picker_cmd_bufs[img_idx]);

	result = vkEndCommandBuffer(picker_cmd_bufs[img_idx]);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to record command buffer.");
	}

	VkCommandBuffer cmd_buffers[] =
	{
		cmd_bufs[img_idx],
		picker_cmd_bufs[img_idx],
	};


	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore swait_semaphores[] = { image_avail_semaphores[frame.frame_idx] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = swait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = cnt_of_array(cmd_buffers);
	submit_info.pCommandBuffers = cmd_buffers;

	VkSemaphore signal_semaphores[] = { render_finished_semaphores[frame.frame_idx] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	result = vkQueueSubmit(dev.get_gfx_queue(), 1, &submit_info, in_flight_fences[frame.frame_idx]);

	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to submit draw command buffer.");
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapchains[] = { handle };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &img_idx;
	present_info.pResults = NULL; // Optional

	result = vkQueuePresentKHR(dev.get_present_queue(), &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR
		|| result == VK_SUBOPTIMAL_KHR)
	{
		resize(_extent);
		return;
	}
	else if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to acquire swap chain image.");
	}
}

VkCommandBuffer vlk_swapchain::get_cmd_buf(const vlk_frame& frame) const
{
	return cmd_bufs[frame.image_idx];
}

VkImage vlk_swapchain::get_picker_image(const vlk_frame& frame) const
{
	return picker_images[frame.image_idx];
}

VkExtent2D vlk_swapchain::get_extent() const
{
	return _extent;
}

void vlk_swapchain::recreate(int width, int height)
{
	VkExtent2D extent;
	extent.width = width;
	extent.height = height;

	resize(extent);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_swapchain::begin_picker_render_pass(vlk_frame& frame)
{
	/*
	Begin picker buffer render pass command buffer
	*/
	frame.picker_cmd_buf = picker_cmd_bufs[frame.image_idx];

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(frame.picker_cmd_buf, &begin_info) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to begin recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = dev.get_picker_render_pass();
	render_pass_info.framebuffer = picker_frame_bufs[frame.image_idx];
	render_pass_info.renderArea.offset.x = 0;
	render_pass_info.renderArea.offset.y = 0;
	render_pass_info.renderArea.extent = _extent;

	/* Initialize picker buffer to 0xFFFFFFFF. This is the invalid entity id. */
	VkClearValue clear_values[1];
	memset(&clear_values, 0, sizeof(clear_values));
	clear_values[0].color.float32[0] = 1.0f;
	clear_values[0].color.float32[1] = 1.0f;
	clear_values[0].color.float32[2] = 1.0f;
	clear_values[0].color.float32[3] = 1.0f;

	render_pass_info.clearValueCount = cnt_of_array(clear_values);
	render_pass_info.pClearValues = clear_values;

	vkCmdBeginRenderPass(frame.picker_cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void vlk_swapchain::begin_primary_render_pass(vlk_frame& frame)
{
	/*
	Begin command buffer
	*/
	frame.cmd_buf = cmd_bufs[frame.image_idx];

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	begin_info.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(frame.cmd_buf, &begin_info) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to begin recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = dev.get_render_pass();
	render_pass_info.framebuffer = frame_bufs[frame.image_idx];
	render_pass_info.renderArea.offset.x = 0;
	render_pass_info.renderArea.offset.y = 0;
	render_pass_info.renderArea.extent = _extent;

	VkClearValue clear_values[2];
	memset(&clear_values, 0, sizeof(clear_values));
	clear_values[0].color.float32[0] = 0.0f;
	clear_values[0].color.float32[1] = 0.0f;
	clear_values[0].color.float32[2] = 0.0f;
	clear_values[0].color.float32[3] = 1.0f;
	clear_values[1].depthStencil.depth = 1.0f;
	clear_values[1].depthStencil.stencil = 0;

	render_pass_info.clearValueCount = cnt_of_array(clear_values);
	render_pass_info.pClearValues = clear_values;

	vkCmdBeginRenderPass(frame.cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

VkExtent2D vlk_swapchain::choose_swap_extent(VkExtent2D desired_extent, VkSurfaceCapabilitiesKHR* capabilities) const
{
    VkExtent2D extent;

	if (capabilities->currentExtent.width != UINT32_MAX)
	{
		/* extent already defined by surface */
		extent = capabilities->currentExtent;
	}
	else
	{
		/* swap chain must set extent */
		extent = desired_extent;
	}

    return extent;
}

void vlk_swapchain::create_all(VkExtent2D extent)
{
	_extent = extent;

	/*
	Order matters for these. These are recreated on resize.
	*/
	create_swapchain(extent);
	create_image_views();
	create_depth_buffer();
	create_picker_buffers();
	create_framebuffers();

	/*
	Order doesn't matter for these. Does not need recreated on resize.
	*/
	create_command_buffers();
	create_semaphores();
}

void vlk_swapchain::create_command_buffers()
{
	cmd_bufs.resize(gpu::num_frame_buf);

	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = dev.get_cmd_pool();
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)cmd_bufs.size();

	if (vkAllocateCommandBuffers(dev.get_handle(), &alloc_info, cmd_bufs.data()) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate command buffers.");
	}

	/*
	Create command buffers for picker buffer render pass
	*/
	picker_cmd_bufs.resize(gpu::num_frame_buf);

	alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = dev.get_cmd_pool();
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = (uint32_t)picker_cmd_bufs.size();

	if (vkAllocateCommandBuffers(dev.get_handle(), &alloc_info, picker_cmd_bufs.data()) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate command buffers.");
	}
}

void vlk_swapchain::create_depth_buffer()
{
	VkResult result;
	VkFormat depth_format = gpu.get_depth_format();
	depth_images.resize(gpu::num_frame_buf);
	depth_image_views.resize(gpu::num_frame_buf);
	depth_image_allocations.resize(gpu::num_frame_buf);

	for (size_t i = 0; i < depth_images.size(); i++)
	{
		/*
		Create image
		*/
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = _extent.width;
		image_info.extent.height = _extent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;

		image_info.format = depth_format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0; // Optional

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		/* https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/34 */
		alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		result = vmaCreateImage(dev.get_allocator(), &image_info, &alloc_info, &depth_images[i], &depth_image_allocations[i], NULL);
		if (result != VK_SUCCESS) 
		{
			LOG_FATAL("Failed to create depth buffer image.");
		}

		/*
		Create image view
		*/
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = depth_images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(dev.get_handle(), &view_info, NULL, &depth_image_views[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create depth texture image view.");
		}

		dev.transition_image_layout(depth_images[i], depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

void vlk_swapchain::create_framebuffers()
{
	uint32_t i;
	frame_bufs.resize(gpu::num_frame_buf);

	for (i = 0; i < frame_bufs.size(); i++) 
	{
		VkImageView attachments[] =
		{
			image_views[i],
			depth_image_views[i],
		};

		VkFramebufferCreateInfo framebuf_info = {};
		framebuf_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuf_info.renderPass = dev.get_render_pass();
		framebuf_info.attachmentCount = cnt_of_array(attachments);
		framebuf_info.pAttachments = attachments;
		framebuf_info.width = _extent.width;
		framebuf_info.height = _extent.height;
		framebuf_info.layers = 1;

		if (vkCreateFramebuffer(dev.get_handle(), &framebuf_info, NULL, &frame_bufs[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create framebuffer.");
		}
	}
}

void vlk_swapchain::create_image_views()
{
	uint32_t i;
	image_views.resize(gpu::num_frame_buf);

	for (i = 0; i < image_views.size(); i++) 
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = images[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = surface_format.format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(dev.get_handle(), &create_info, NULL, &image_views[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create image views.");
		}
	}
}

void vlk_swapchain::create_picker_buffers()
{
	picker_images.resize(gpu::num_frame_buf);
	picker_image_allocations.resize(gpu::num_frame_buf);
	picker_image_views.resize(gpu::num_frame_buf);
	picker_frame_bufs.resize(gpu::num_frame_buf);

	for (int i = 0; i < picker_images.size(); ++i)
	{
		/*
		Create image
		*/
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = _extent.width;
		image_info.extent.height = _extent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;

		image_info.format = VK_FORMAT_B8G8R8A8_UNORM;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.flags = 0;

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		alloc_info.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		VkResult result = vmaCreateImage(dev.get_allocator(), &image_info, &alloc_info, &picker_images[i], &picker_image_allocations[i], NULL);
		if (result != VK_SUCCESS) 
		{
			LOG_FATAL("Failed to create picker buffer image.");
		}
		
		dev.transition_image_layout(picker_images[i], image_info.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		/*
		Create image view
		*/
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = picker_images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = image_info.format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(dev.get_handle(), &view_info, NULL, &picker_image_views[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create picker buffer image view.");
		}

		/*
		Create frame buffers
		*/
		VkImageView attachments[] =
		{
			picker_image_views[i],
		};

		VkFramebufferCreateInfo framebuf_info = {};
		framebuf_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuf_info.renderPass = dev.get_picker_render_pass();
		framebuf_info.attachmentCount = cnt_of_array(attachments);
		framebuf_info.pAttachments = attachments;
		framebuf_info.width = _extent.width;
		framebuf_info.height = _extent.height;
		framebuf_info.layers = 1;

		if (vkCreateFramebuffer(dev.get_handle(), &framebuf_info, NULL, &picker_frame_bufs[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create framebuffer.");
		}
	}
}

void vlk_swapchain::create_semaphores()
{
	VkDevice device = dev.get_handle();
	image_avail_semaphores.resize(gpu::num_frame_buf);
	render_finished_semaphores.resize(gpu::num_frame_buf);
	in_flight_fences.resize(gpu::num_frame_buf);

	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < in_flight_fences.size(); i++)
	{
		if (vkCreateSemaphore(device, &semaphore_info, NULL, &image_avail_semaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(device, &semaphore_info, NULL, &render_finished_semaphores[i]) != VK_SUCCESS
			|| vkCreateFence(device, &fence_info, NULL, &in_flight_fences[i]) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create semaphores for a frame.");
		}
	}
}

void vlk_swapchain::create_swapchain(VkExtent2D extent)
{
	uint32_t image_count = gpu::num_frame_buf;

	/*
	Get surface capabilties
	*/
	VkSurfaceCapabilitiesKHR surface_capabilities = {};
	VkResult result = gpu.query_surface_capabilties(surface, surface_capabilities);

	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to query surface capabilities.");
	}

	/*
	Determine swapchain properties
	*/
	present_mode = gpu.optimal_present_mode;
	surface_format = gpu.optimal_surface_format;
	extent = choose_swap_extent(extent, &surface_capabilities);

	/*
	Build create info for the swap chain
	*/
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	/* queue family sharing */
	uint32_t queueFamilyIndices[] = 
	{
		dev.get_gfx_family_idx(), 
		dev.get_present_family_idx()
	};

	if (dev.get_gfx_family_idx() != dev.get_present_family_idx())
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else 
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0; // Optional
		create_info.pQueueFamilyIndices = NULL; // Optional
	}

	create_info.preTransform = surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	/*
	Create the swap chain
	*/
	result = vkCreateSwapchainKHR(dev.get_handle(), &create_info, NULL, &handle);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create swap chain.");
	}

	/*
	Get swap chain images
	*/
	vkGetSwapchainImagesKHR(dev.get_handle(), handle, &image_count, NULL);
	images.resize(image_count);
	vkGetSwapchainImagesKHR(dev.get_handle(), handle, &image_count, images.data());
}

void vlk_swapchain::destroy_all()
{
	destroy_semaphores();
	destroy_command_buffers();

	destroy_framebuffers();
	destroy_picker_buffers();
	destroy_depth_buffer();
	destroy_image_views();
	destroy_swapchain();
}

void vlk_swapchain::destroy_command_buffers()
{
	/*
	Do nothing. Command buffers are cleaned up automatically by their command pool.
	*/
}

void vlk_swapchain::destroy_depth_buffer()
{
	uint32_t i;

	for (i = 0; i < images.size(); i++)
	{
		vkDestroyImageView(dev.get_handle(), depth_image_views[i], NULL);
		vmaDestroyImage(dev.get_allocator(), depth_images[i], depth_image_allocations[i]);
	}
}

void vlk_swapchain::destroy_framebuffers()
{
	uint32_t i;

	for (i = 0; i < frame_bufs.size(); ++i)
	{
		vkDestroyFramebuffer(dev.get_handle(), frame_bufs[i], NULL);
	}
}

void vlk_swapchain::destroy_image_views()
{
	uint32_t i;

	for (i = 0; i < image_views.size(); ++i)
	{
		vkDestroyImageView(dev.get_handle(), image_views[i], NULL);
	}
}

void vlk_swapchain::destroy_picker_buffers()
{
	uint32_t i;

	for (i = 0; i < picker_images.size(); i++)
	{
		vkDestroyFramebuffer(dev.get_handle(), picker_frame_bufs[i], NULL);
		vkDestroyImageView(dev.get_handle(), picker_image_views[i], NULL);
		vmaDestroyImage(dev.get_allocator(), picker_images[i], picker_image_allocations[i]);
	}
}

void vlk_swapchain::destroy_semaphores()
{
	for (size_t i = 0; i < in_flight_fences.size(); i++)
	{
		vkDestroySemaphore(dev.get_handle(), render_finished_semaphores[i], NULL);
		vkDestroySemaphore(dev.get_handle(), image_avail_semaphores[i], NULL);
		vkDestroyFence(dev.get_handle(), in_flight_fences[i], NULL);
	}
}

void vlk_swapchain::destroy_swapchain()
{
	vkDestroySwapchainKHR(dev.get_handle(), handle, NULL);
}

void vlk_swapchain::resize(VkExtent2D extent)
{
	_extent = extent;

	/* wait until device is idle before recreation */
	vkDeviceWaitIdle(dev.get_handle());

	/* destroy things that need recreated */
	destroy_framebuffers();
	destroy_picker_buffers();
	destroy_depth_buffer();
	destroy_image_views();
	destroy_swapchain();

	/* re-create resources */
	create_swapchain(extent);
	create_image_views();
	create_depth_buffer();
	create_picker_buffers();
	create_framebuffers();
}

}   /* namespace jetz */
