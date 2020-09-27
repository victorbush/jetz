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

void vlk_material::bind() const
{
	LOG_FATAL("TODO");
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

		VkWriteDescriptorSet descriptor_writes[2];
		memset(descriptor_writes, 0, sizeof(descriptor_writes));

		/* Material UBO */
		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = _sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		/* Base color texture */
		auto base_color_texture = _base_color_texture.lock();
		if (base_color_texture != nullptr)
		{
			descriptor_writes[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[writeIdx].dstSet = _sets[i];
			descriptor_writes[writeIdx].dstBinding = 1;
			descriptor_writes[writeIdx].dstArrayElement = 0;
			descriptor_writes[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[writeIdx].descriptorCount = 1;
			descriptor_writes[writeIdx].pImageInfo = baseColorTexture->GetImageInfo();
			++writeIdx;
		}

		/* Metallic/roughness texture */
		auto metallicRoughnessTexture = mat->MetallicRoughnessTexture.lock();
		if (metallicRoughnessTexture != nullptr)
		{
			descriptorWrites[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[writeIdx].dstSet = _sets[i];
			descriptorWrites[writeIdx].dstBinding = 2;
			descriptorWrites[writeIdx].dstArrayElement = 0;
			descriptorWrites[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[writeIdx].descriptorCount = 1;
			descriptorWrites[writeIdx].pImageInfo = metallicRoughnessTexture->GetImageInfo();
			++writeIdx;
		}

		/* Normal texture */
		auto normalTexture = mat->NormalTexture.lock();
		if (normalTexture != nullptr)
		{
			descriptorWrites[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[writeIdx].dstSet = _sets[i];
			descriptorWrites[writeIdx].dstBinding = 3;
			descriptorWrites[writeIdx].dstArrayElement = 0;
			descriptorWrites[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[writeIdx].descriptorCount = 1;
			descriptorWrites[writeIdx].pImageInfo = normalTexture->GetImageInfo();
			++writeIdx;
		}

		/* Occlusion texture */
		// TODO
		
		/* Emissive texture */
		auto emissiveTexture = mat->EmissiveTexture.lock();
		if (emissiveTexture != nullptr)
		{
			descriptorWrites[writeIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[writeIdx].dstSet = _sets[i];
			descriptorWrites[writeIdx].dstBinding = 5;
			descriptorWrites[writeIdx].dstArrayElement = 0;
			descriptorWrites[writeIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[writeIdx].descriptorCount = 1;
			descriptorWrites[writeIdx].pImageInfo = emissiveTexture->GetImageInfo();
			++writeIdx;
		}

		vkUpdateDescriptorSets(set->layout->dev->handle, cnt_of_array(descriptor_writes), descriptor_writes, 0, NULL);
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
