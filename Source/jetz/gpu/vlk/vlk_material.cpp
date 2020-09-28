/*=============================================================================
vlk_material.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"
#include "jetz/gpu/vlk/vlk_material.h"
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
	_roughness_factor(1.0f)
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
	auto layouts = std::vector<VkDescriptorSetLayout>(gpu::num_frame_buf, _layout->get_handle());

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = _layout->get_pool_handle();
	alloc_info.descriptorSetCount = layouts.size();
	alloc_info.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(_device.get_handle(), &alloc_info, &_sets[0]);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < _buffers.size(); i++)
	{
		VkDescriptorBufferInfo buffer_info = _buffers[i]->get_buffer_info();

		std::vector<VkWriteDescriptorSet> descriptor_writes;
		int num_writes = 0;
		descriptor_writes.resize(6);

		/* Material UBO */
		descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[num_writes].dstSet = _sets[i];
		descriptor_writes[num_writes].dstBinding = 0;
		descriptor_writes[num_writes].dstArrayElement = 0;
		descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[num_writes].descriptorCount = 1;
		descriptor_writes[num_writes].pBufferInfo = &buffer_info;

		/* Base color texture */
		auto base_color_texture = _base_color_texture.lock();
		if (base_color_texture != nullptr)
		{
			descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[num_writes].dstSet = _sets[i];
			descriptor_writes[num_writes].dstBinding = 1;
			descriptor_writes[num_writes].dstArrayElement = 0;
			descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[num_writes].descriptorCount = 1;
			descriptor_writes[num_writes].pImageInfo = base_color_texture->get_image_info();
			++num_writes;
		}

		/* Metallic/roughness texture */
		auto metallic_roughness_texture = _metallic_roughness_texture.lock();
		if (metallic_roughness_texture != nullptr)
		{
			descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[num_writes].dstSet = _sets[i];
			descriptor_writes[num_writes].dstBinding = 2;
			descriptor_writes[num_writes].dstArrayElement = 0;
			descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[num_writes].descriptorCount = 1;
			descriptor_writes[num_writes].pImageInfo = metallic_roughness_texture->get_image_info();
			++num_writes;
		}

		/* Normal texture */
		auto normal_texture = _normal_texture.lock();
		if (normal_texture != nullptr)
		{
			descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[num_writes].dstSet = _sets[i];
			descriptor_writes[num_writes].dstBinding = 3;
			descriptor_writes[num_writes].dstArrayElement = 0;
			descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[num_writes].descriptorCount = 1;
			descriptor_writes[num_writes].pImageInfo = normal_texture->get_image_info();
			++num_writes;
		}

		/* Occlusion texture */
		auto occlusion_texture = _occlusion_texture.lock();
		if (occlusion_texture != nullptr)
		{
			descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[num_writes].dstSet = _sets[i];
			descriptor_writes[num_writes].dstBinding = 4;
			descriptor_writes[num_writes].dstArrayElement = 0;
			descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[num_writes].descriptorCount = 1;
			descriptor_writes[num_writes].pImageInfo = occlusion_texture->get_image_info();
			++num_writes;
		}

		/* Emissive texture */
		auto emissive_texture = _emissive_texture.lock();
		if (emissive_texture != nullptr)
		{
			descriptor_writes[num_writes].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[num_writes].dstSet = _sets[i];
			descriptor_writes[num_writes].dstBinding = 5;
			descriptor_writes[num_writes].dstArrayElement = 0;
			descriptor_writes[num_writes].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[num_writes].descriptorCount = 1;
			descriptor_writes[num_writes].pImageInfo = emissive_texture->get_image_info();
			++num_writes;
		}

		assert(num_writes <= descriptor_writes.size());
		vkUpdateDescriptorSets(_device.get_handle(), num_writes, descriptor_writes.data(), 0, NULL);
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

}   /* namespace jetz */
