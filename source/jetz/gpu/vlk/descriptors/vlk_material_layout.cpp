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
	pool_sizes[0].descriptorCount = gpu::max_num_materials;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = gpu::max_num_materials;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = cnt_of_array(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	// TOOD : what should maxSets be??
	pool_info.maxSets = gpu::max_num_materials;

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

	/* Base color texture */
	VkDescriptorSetLayoutBinding base_color_texture_binding = {};
	base_color_texture_binding.binding = 1;
	base_color_texture_binding.descriptorCount = 1;
	base_color_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	base_color_texture_binding.pImmutableSamplers = nullptr;
	base_color_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	/* Metallic/roughness texture */
	VkDescriptorSetLayoutBinding metallic_roughness_texture_binding = {};
	metallic_roughness_texture_binding.binding = 2;
	metallic_roughness_texture_binding.descriptorCount = 1;
	metallic_roughness_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	metallic_roughness_texture_binding.pImmutableSamplers = nullptr;
	metallic_roughness_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	/* Normal texture */
	VkDescriptorSetLayoutBinding normal_texture_binding = {};
	normal_texture_binding.binding = 3;
	normal_texture_binding.descriptorCount = 1;
	normal_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normal_texture_binding.pImmutableSamplers = nullptr;
	normal_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	/* Occlusion texture */
	VkDescriptorSetLayoutBinding occlusion_texture_binding = {};
	occlusion_texture_binding.binding = 4;
	occlusion_texture_binding.descriptorCount = 1;
	occlusion_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	occlusion_texture_binding.pImmutableSamplers = nullptr;
	occlusion_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	/* Emissive texture */
	VkDescriptorSetLayoutBinding emissive_texture_binding = {};
	emissive_texture_binding.binding = 5;
	emissive_texture_binding.descriptorCount = 1;
	emissive_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	emissive_texture_binding.pImmutableSamplers = nullptr;
	emissive_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector< VkDescriptorSetLayoutBinding> bindings =
	{
		ubo_layout_binding,
		base_color_texture_binding,
		metallic_roughness_texture_binding,
		normal_texture_binding,
		occlusion_texture_binding,
		emissive_texture_binding
	};

	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = (uint32_t)bindings.size();
	layout_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(dev.get_handle(), &layout_info, NULL, &handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create material descriptor set layout.");
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
