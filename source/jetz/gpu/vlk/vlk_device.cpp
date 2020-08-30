/*=============================================================================
vlk_device.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
DECLARATIONS
=============================================================================*/

/** Checks if a format has a stencil component. */
static bool has_stencil_component(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_device::vlk_device
	(
	vlk_gpu&							gpu,			/* physical device used by the logical device */
	const std::vector<const char*>&		req_dev_ext,	/* required device extensions */
	const std::vector<const char*>&		req_inst_layers	/* required instance layers */
	)
	: gpu(gpu)
{
	gfx_family_idx = -1;
	present_family_idx = -1;

	create_logical_device(req_dev_ext, req_inst_layers);
	create_command_pool();
	create_allocator();
	create_texture_sampler();
	create_layouts();
	create_render_pass();
	create_picker_render_pass();
}

vlk_device::~vlk_device()
{
	destroy_picker_render_pass();
	destroy_render_pass();
	destroy_layouts();
	destroy_texture_sampler();
	destroy_allocator();
	destroy_command_pool();
	destroy_logical_device();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

VkCommandBuffer vlk_device::begin_one_time_cmd_buf() const
{
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer cmd_buf = {};
	vkAllocateCommandBuffers(handle, &alloc_info, &cmd_buf);

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmd_buf, &begin_info);

	return cmd_buf;
}

void vlk_device::copy_buffer_to_img_now
	(
	VkBuffer	buffer, 
	VkImage		image, 
	uint32_t	width, 
	uint32_t	height
	) const
{
	VkCommandBuffer cmd_buf = begin_one_time_cmd_buf();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageOffset.z = 0;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(
		cmd_buf,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	end_one_time_cmd_buf(cmd_buf);
}

VkShaderModule vlk_device::create_shader(const std::string& file) const
{
	LOG_FATAL("NOT IMPLEMENTED");
	return VkShaderModule();
}

void vlk_device::destroy_shader(VkShaderModule shader) const
{
	vkDestroyShaderModule(handle, shader, NULL);
}

void vlk_device::end_one_time_cmd_buf(VkCommandBuffer cmd_buf) const
{
	vkEndCommandBuffer(cmd_buf);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmd_buf;

	vkQueueSubmit(gfx_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(gfx_queue);

	vkFreeCommandBuffers(handle, command_pool, 1, &cmd_buf);
}

VmaAllocator vlk_device::get_allocator() const { return allocator; }

VkCommandPool vlk_device::get_cmd_pool() const { return command_pool; }

vlk_gpu& vlk_device::get_gpu() const { return gpu; }

VkDevice vlk_device::get_handle() const { return handle; }

VkRenderPass vlk_device::get_render_pass() const { return render_pass; }

VkRenderPass vlk_device::get_picker_render_pass() const { return picker_render_pass; }

int vlk_device::get_gfx_family_idx() const { return gfx_family_idx; }

VkQueue vlk_device::get_gfx_queue() const { return gfx_queue; }

vlk_material_layout& vlk_device::get_material_layout() const { return *material_layout; }

vlk_per_view_layout& vlk_device::get_per_view_layout() const { return *per_view_layout; }

int vlk_device::get_present_family_idx() const { return present_family_idx; }

VkQueue vlk_device::get_present_queue() const { return present_queue; }

VkSampler vlk_device::get_texture_sampler() const { return texture_sampler; }

void vlk_device::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) const
{
	VkCommandBuffer cmd_buf = begin_one_time_cmd_buf();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;

	if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (has_stencil_component(format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else
	{
		LOG_FATAL("Unsupported layout transition.");
	}

	vkCmdPipelineBarrier(
		cmd_buf,
		sourceStage, destinationStage,
		0,
		0, NULL,
		0, NULL,
		1, &barrier
	);

	end_one_time_cmd_buf(cmd_buf);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_device::create_allocator()
{
	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = gpu.get_handle();
	allocator_info.device = handle;

	// TODO : can enable this later. Read docs to make sure to enable required extensions.
	//if (VK_KHR_dedicated_allocation_enabled)
	//{
	//    allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	//}

	//VkAllocationCallbacks cpuAllocationCallbacks = {};
	//if (USE_CUSTOM_CPU_ALLOCATION_CALLBACKS)
	//{
	//    cpuAllocationCallbacks.pUserData = CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA;
	//    cpuAllocationCallbacks.pfnAllocation = &CustomCpuAllocation;
	//    cpuAllocationCallbacks.pfnReallocation = &CustomCpuReallocation;
	//    cpuAllocationCallbacks.pfnFree = &CustomCpuFree;
	//    allocatorInfo.pAllocationCallbacks = &cpuAllocationCallbacks;
	//}

	if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create memory allocatory.");
	}
}

void vlk_device::create_command_pool()
{
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = gfx_family_idx;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(handle, &pool_info, NULL, &command_pool) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create command pool.");
	}
}

void vlk_device::create_layouts()
{
	per_view_layout = new jetz::vlk_per_view_layout(*this);
	material_layout = new jetz::vlk_material_layout(*this);
}

void vlk_device::create_logical_device
(
	const std::vector<const char*>& req_dev_ext,		/* required device extensions */
	const std::vector<const char*>& req_inst_layers		/* required instance layers */
)
{
	uint32_t i;

	/*
	Specify what queues to create
	*/
	std::vector<VkDeviceQueueCreateInfo> queues;

	/* Check for graphics family */
	if (gpu.queue_family_indices.graphics_families.size() == 0)
	{
		LOG_FATAL("No graphics family queue found.");
	}

	gfx_family_idx = gpu.queue_family_indices.graphics_families[0];

	/* Check for present family */
	if (gpu.queue_family_indices.present_families.size() == 0)
	{
		LOG_FATAL("No present family queue found.");
	}

	present_family_idx = gpu.queue_family_indices.present_families[0];

	/* Create list of used queue families */
	used_queue_families.push_back(gfx_family_idx);

	/* Queue family list must be unique */
	if (gfx_family_idx != present_family_idx)
	{
		used_queue_families.push_back((uint32_t)present_family_idx);
	}

	float queuePriority = 1.0f;

	/* create multiple queues if needed based on QF properties */
	for (i = 0; i < used_queue_families.size(); ++i)
	{
		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = used_queue_families[i];
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queuePriority;
		queues.push_back(queue_info);
	}

	/*
	* Specify device features to use
	*/
	VkPhysicalDeviceFeatures device_features = {};
	device_features.samplerAnisotropy = gpu.supported_features.samplerAnisotropy;

	/*
	* Create the logical device
	*/
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	/* queues */
	create_info.queueCreateInfoCount = (uint32_t)queues.size();
	create_info.pQueueCreateInfos = queues.data();

	/* device features */
	create_info.pEnabledFeatures = &device_features;

	/* extensions */
	create_info.enabledExtensionCount = (uint32_t)req_dev_ext.size();
	create_info.ppEnabledExtensionNames = req_dev_ext.data();

	/* layers */
	create_info.enabledLayerCount = (uint32_t)req_inst_layers.size();
	create_info.ppEnabledLayerNames = req_inst_layers.data();

	/* create the logical device */
	if (vkCreateDevice(gpu.get_handle(), &create_info, NULL, &handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create logical device.");
	}

	/*
	* Get queue handles
	*/
	vkGetDeviceQueue(handle, gfx_family_idx, 0, &gfx_queue);
	vkGetDeviceQueue(handle, present_family_idx, 0, &present_queue);
}

void vlk_device::create_picker_render_pass()
{
	/*
	Attachment setup
	*/

	/* Primary color attachment */
	VkAttachmentDescription color_attach = {};
	color_attach.format = gpu.optimal_surface_format.format;
	color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attach_ref = {};
	color_attach_ref.attachment = 0;
	color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachments[] =
	{
		color_attach,
	};

	/*
	Subpass setup
	*/

	/* Primary subpass */
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attach_ref;

	VkSubpassDescription subpasses[] = { subpass };

	/*
	Subpass dependencies
	*/
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit subpass before the render pass
	dependency.dstSubpass = 0; // 0 is index of our first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency dependencies[] = { dependency };

	/*
	Renderpass setup
	*/
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = cnt_of_array(attachments);
	render_pass_info.pAttachments = attachments;
	render_pass_info.subpassCount = cnt_of_array(subpasses);
	render_pass_info.pSubpasses = subpasses;
	render_pass_info.dependencyCount = cnt_of_array(dependencies);
	render_pass_info.pDependencies = dependencies;

	if (vkCreateRenderPass(handle, &render_pass_info, NULL, &picker_render_pass) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create render pass.");
	}
}

void vlk_device::create_render_pass()
{
	/*
	Attachment setup
	*/

	/* Primary color attachment */
	VkAttachmentDescription color_attach = {};
	color_attach.format = gpu.optimal_surface_format.format;
	color_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attach_ref = {};
	color_attach_ref.attachment = 0;
	color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	/* Primary depth attachment */
	VkAttachmentDescription depth_attach = {};
	depth_attach.format = gpu.get_depth_format();
	depth_attach.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attach.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attach.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attach_ref = {};
	depth_attach_ref.attachment = 1;
	depth_attach_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription attachments[] =
	{
		color_attach,
		depth_attach,
	};

	/*
	Subpass setup
	*/

	/* Primary subpass */
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attach_ref;
	subpass.pDepthStencilAttachment = &depth_attach_ref;

	VkSubpassDescription subpasses[] = { subpass };

	/*
	Subpass dependencies
	*/
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit subpass before the render pass
	dependency.dstSubpass = 0; // 0 is index of our first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency dependencies[] = { dependency };

	/*
	Renderpass setup
	*/
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = cnt_of_array(attachments);
	render_pass_info.pAttachments = attachments;
	render_pass_info.subpassCount = cnt_of_array(subpasses);
	render_pass_info.pSubpasses = subpasses;
	render_pass_info.dependencyCount = cnt_of_array(dependencies);
	render_pass_info.pDependencies = dependencies;

	if (vkCreateRenderPass(handle, &render_pass_info, NULL, &render_pass) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create render pass.");
	}
}

void vlk_device::create_texture_sampler()
{
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;

	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	if (gpu.supported_features.samplerAnisotropy)
	{
		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = 16.0f;
	}
	else
	{
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 1.0f;
	}

	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;

	if (vkCreateSampler(handle, &sampler_info, NULL, &texture_sampler) != VK_SUCCESS) 
	{
		LOG_FATAL("Failed to create texture sampler.");
	}
}

void vlk_device::destroy_allocator()
{
	vmaDestroyAllocator(allocator);
}

void vlk_device::destroy_command_pool()
{
	vkDestroyCommandPool(handle, command_pool, NULL);
}

void vlk_device::destroy_layouts()
{
	delete per_view_layout;
	delete material_layout;
}

void vlk_device::destroy_logical_device()
{
	vkDestroyDevice(handle, NULL);
}

void vlk_device::destroy_picker_render_pass()
{
	vkDestroyRenderPass(handle, picker_render_pass, NULL);
}

void vlk_device::destroy_render_pass()
{
	vkDestroyRenderPass(handle, render_pass, NULL);
}

void vlk_device::destroy_texture_sampler()
{
	vkDestroySampler(handle, texture_sampler, NULL);
}

}   /* namespace jetz */
