/*=============================================================================
vlk_material_layout.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/descriptors/vlk_material_layout.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_material_layout::vlk_material_layout(vlk_device& dev)
	: vlk_descriptor_layout(dev)
{
	create_descriptor_pool();
	create_layout();
}

vlk_material_layout::~vlk_material_layout()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

/*=============================================================================
PROTECTED METHODS
=============================================================================*/

void vlk_material_layout::create_descriptor_pool()
{
	VkDescriptorPoolSize pool_sizes[2];
	memset(pool_sizes, 0, sizeof(pool_sizes));
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = vlk::max_num_materials;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = vlk::max_num_materials;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = cnt_of_array(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	// TOOD : what should maxSets be??
	pool_info.maxSets = vlk::max_num_materials;

	if (vkCreateDescriptorPool(dev.get_handle(), &pool_info, NULL, &pool_handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create descriptor pool.");
	}
}

void vlk_material_layout::create_layout()
{
	/* Material UBO */
	VkDescriptorSetLayoutBinding ubo_layout_binding = {};
	ubo_layout_binding.binding = 0;
	ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_layout_binding.descriptorCount = 1;
	ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	ubo_layout_binding.pImmutableSamplers = NULL;

	/* Diffuse texture */
	VkDescriptorSetLayoutBinding diffuse_texture_binding = {};
	diffuse_texture_binding.binding = 1;
	diffuse_texture_binding.descriptorCount = 1;
	diffuse_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuse_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	diffuse_texture_binding.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutBinding bindings[2];
	memset(bindings, 0, sizeof(bindings));
	bindings[0] = ubo_layout_binding;
	bindings[1] = diffuse_texture_binding;

	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = cnt_of_array(bindings);
	layout_info.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(dev.get_handle(), &layout_info, NULL, &handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create descriptor set layout.");
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
