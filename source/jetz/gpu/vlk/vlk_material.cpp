/*=============================================================================
vlk_material.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"
#include "jetz/gpu/vlk/vlk_material.h"
#include "jetz/gpu/vlk/descriptors/vlk_material_layout.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_material::vlk_material
	(
	vlk_device&							device,
	const vlk_material_create_info&		create_info
	)
	: 
	_device(device),
	_base_color_factor(1.0f, 1.0f, 1.0f, 1.0f),
	_emissive_factor(0.0f, 0.0f, 0.0f),
	_metallic_factor(1.0f),
	_roughness_factor(1.0f),
	_default_texture(device.get_default_texture())
{
	_emissive_factor = create_info.emissive_factor;
	_emissive_texture = create_info.emissive_texture;
	_normal_texture = create_info.normal_texture;
	_base_color_factor = create_info.base_color_factor;
	_base_color_texture = create_info.base_color_texture;
	_metallic_factor = create_info.metallic_factor;
	_metallic_roughness_texture = create_info.metallic_roughness_texture;
	_roughness_factor = create_info.roughness_factor;

	/* Create the UBOs and descriptor set and update them with the material properties/textures */
	create_buffers();
	create_sets();
}

vlk_material::~vlk_material()
{
	destroy_sets();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_material::bind
	(
	const vlk_frame&			frame,
	VkPipelineLayout			pipeline_layout
	) const
{
	/* Set num is specified in the shader */
	uint32_t set_num = 1;
	vkCmdBindDescriptorSets(frame.cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, set_num, 1, &_sets[frame.image_idx], 0, NULL);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_material::create_buffers()
{
	VkDeviceSize buffer_size = sizeof(vlk_material_ubo);
	_buffers.clear();

	/* Setup data to send to GPU */
	vlk_material_ubo ubo = {};
	ubo.base_color_factor = _base_color_factor;
	ubo.emissive_factor = _emissive_factor;
	ubo.metallic_factor = _metallic_factor;
	ubo.roughness_factor = _roughness_factor;
	ubo.hasBaseColorTexture = !_base_color_texture.expired();
	ubo.hasEmissiveTexture = !_emissive_texture.expired();
	ubo.hasMetallicRoughnessTexture = !_metallic_roughness_texture.expired();
	ubo.hasNormalTexture = !_normal_texture.expired();
	ubo.hasOcclusionTexture = !_occlusion_texture.expired();

	/* Create a UBO for all frames */
	for (uint8_t i = 0; i < gpu::num_frame_buf; ++i)
	{
		auto buffer = new vlk_buffer(_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		_buffers.push_back(uptr<vlk_buffer>(buffer));

		/* Populate the buffer on the GPU */
		buffer->update((void*)&ubo, 0, buffer_size);
	}
}

void vlk_material::create_sets()
{
	/*
	Create a descriptor set for each possible concurrent frame
	*/
	_sets.resize(gpu::num_frame_buf);

	auto& layout = _device.get_material_layout();
	auto layout_handles = std::vector<VkDescriptorSetLayout>(gpu::num_frame_buf, layout.get_handle());

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = layout.get_pool_handle();
	alloc_info.descriptorSetCount = layout_handles.size();
	alloc_info.pSetLayouts = layout_handles.data();

	VkResult result = vkAllocateDescriptorSets(_device.get_handle(), &alloc_info, _sets.data());
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < _buffers.size(); i++)
	{
		VkDescriptorBufferInfo buffer_info = _buffers[i]->get_buffer_info();

		std::vector<VkWriteDescriptorSet> descriptor_writes;
		descriptor_writes.resize(6);

		/* Material UBO */
		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = _sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		/* Base color texture */
		descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[1].dstSet = _sets[i];
		descriptor_writes[1].dstBinding = 1;
		descriptor_writes[1].dstArrayElement = 0;
		descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[1].descriptorCount = 1;
		descriptor_writes[1].pImageInfo = get_img_info(_base_color_texture);

		/* Metallic/roughness texture */
		descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[2].dstSet = _sets[i];
		descriptor_writes[2].dstBinding = 2;
		descriptor_writes[2].dstArrayElement = 0;
		descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[2].descriptorCount = 1;
		descriptor_writes[2].pImageInfo = get_img_info(_metallic_roughness_texture);

		/* Normal texture */
		descriptor_writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[3].dstSet = _sets[i];
		descriptor_writes[3].dstBinding = 3;
		descriptor_writes[3].dstArrayElement = 0;
		descriptor_writes[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[3].descriptorCount = 1;
		descriptor_writes[3].pImageInfo = get_img_info(_normal_texture);

		/* Occlusion texture */
		descriptor_writes[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[4].dstSet = _sets[i];
		descriptor_writes[4].dstBinding = 4;
		descriptor_writes[4].dstArrayElement = 0;
		descriptor_writes[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[4].descriptorCount = 1;
		descriptor_writes[4].pImageInfo = get_img_info(_occlusion_texture);

		/* Emissive texture */
		descriptor_writes[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[5].dstSet = _sets[i];
		descriptor_writes[5].dstBinding = 5;
		descriptor_writes[5].dstArrayElement = 0;
		descriptor_writes[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[5].descriptorCount = 1;
		descriptor_writes[5].pImageInfo = get_img_info(_emissive_texture);

		vkUpdateDescriptorSets(_device.get_handle(), descriptor_writes.size(), descriptor_writes.data(), 0, NULL);
	}
}

void vlk_material::destroy_buffers()
{
	_buffers.clear();
}

void vlk_material::destroy_sets()
{
	/*
	Descriptor sets are destroyed automatically when the parent descriptor pool
	is destroyed.
	*/
}

/** Gets the image info for a texture. If the texture is null, the default texture is used. */
VkDescriptorImageInfo* vlk_material::get_img_info(wptr<vlk_texture> tex)
{
	auto t = tex.lock();
	return t ? t->get_image_info() : _default_texture.lock()->get_image_info();
}

}   /* namespace jetz */
