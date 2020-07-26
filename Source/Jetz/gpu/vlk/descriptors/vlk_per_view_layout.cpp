/*=============================================================================
vlk_per_view_layout.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_layout.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_per_view_layout::vlk_per_view_layout(vlk_device& dev)
	: dev(dev)
{
	create_descriptor_pool();
	create_layout();
}

vlk_per_view_layout::~vlk_per_view_layout()
{
	destroy_layout();
	destroy_descriptor_pool();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

VkDescriptorSetLayout vlk_per_view_layout::get_handle() const
{
	return handle;
}

vlk_device& vlk_per_view_layout::get_device() const
{
	return dev;
}

VkDescriptorPool vlk_per_view_layout::get_pool_handle() const
{
	return pool_handle;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_per_view_layout::create_descriptor_pool()
{
	VkDescriptorPoolSize pool_sizes[1];
	memset(pool_sizes, 0, sizeof(pool_sizes));
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = vlk::num_frame_buf;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = cnt_of_array(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	// TOOD : what should maxSets be??
	pool_info.maxSets = vlk::num_frame_buf;

	if (vkCreateDescriptorPool(dev.get_handle(), &pool_info, NULL, &pool_handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create descriptor pool.");
	}
}

void vlk_per_view_layout::create_layout()
{
	VkDescriptorSetLayoutBinding ubo_layout_binding = {};
	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	ubo_layout_binding.pImmutableSamplers = NULL; // Optional

	VkDescriptorSetLayoutBinding bindings[1];
	memset(bindings, 0, sizeof(bindings));
	bindings[0] = ubo_layout_binding;

	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = cnt_of_array(bindings);
	layout_info.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(dev.get_handle(), &layout_info, NULL, &handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create descriptor set layout.");
	}
}

void vlk_per_view_layout::destroy_descriptor_pool()
{
	vkDestroyDescriptorPool(dev.get_handle(), pool_handle, NULL);
}

void vlk_per_view_layout::destroy_layout()
{
	vkDestroyDescriptorSetLayout(dev.get_handle(), handle, NULL);
}

}   /* namespace jetz */
