/*=============================================================================
vlk_per_view_set.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_layout.h"
#include "jetz/gpu/vlk/descriptors/vlk_per_view_set.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_per_view_set::vlk_per_view_set(vlk_per_view_layout& layout)
	: layout(layout)
{
	create_buffers();
	create_sets();
}

vlk_per_view_set::~vlk_per_view_set()
{
	destroy_sets();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_per_view_set::_vlk_per_view_set__bind
	(
	VkCommandBuffer			cmd_buf, 
	const vlk_frame&		frame, 
	VkPipelineLayout		pipelineLayout
	)
{
	uint32_t setNum = 0; // TODO : hardcoded for now
	vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, setNum, 1, &sets[frame.image_idx], 0, NULL);
}

void vlk_per_view_set::_vlk_per_view_set__update
	(
	const vlk_frame&		frame,
	const camera&			camera,
	VkExtent2D				extent
	)
{
	vlk_per_view_ubo ubo = {};

	/* View matrix */
	ubo.view = camera.get_view_matrix();

	/* Projection matrix */
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	/* Camera position */
	ubo.camera_pos = camera.get_pos();

	/* Update the UBO */
	buffers[frame.image_idx]->update((void*)&ubo, 0, sizeof(ubo));
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_per_view_set::create_buffers()
{
	VkDeviceSize buffer_size = sizeof(vlk_per_view_ubo);
	buffers.resize(vlk::num_frame_buf);

	for (uint32_t i = 0; i < buffers.size(); ++i)
	{
		buffers[i] = new jetz::vlk_buffer(layout.get_device(), buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}
}

void vlk_per_view_set::create_sets()
{
	/*
	Create a descriptor set for each possible concurrent frame
	*/
	sets.resize(vlk::num_frame_buf);

	std::vector<VkDescriptorSetLayout> layouts(vlk::num_frame_buf, layout.get_handle());
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = layout.get_pool_handle();
	alloc_info.descriptorSetCount = layouts.size();
	alloc_info.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(layout.get_device().get_handle(), &alloc_info, &sets[0]);
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate descriptor sets.");
	}

	for (uint32_t i = 0; i < vlk::num_frame_buf; i++)
	{
		VkDescriptorBufferInfo buffer_info = buffers[i]->get_buffer_info();

		VkWriteDescriptorSet descriptor_writes[1];
		memset(descriptor_writes, 0, sizeof(descriptor_writes));

		descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[0].dstSet = sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		vkUpdateDescriptorSets(layout.get_device().get_handle(), cnt_of_array(descriptor_writes), descriptor_writes, 0, NULL);
	}
}

void vlk_per_view_set::destroy_buffers()
{
	for (uint32_t i = 0; i < buffers.size(); ++i)
	{
		delete buffers[i];
	}

	buffers.clear();
}

void vlk_per_view_set::destroy_sets()
{
	/* 
	Descriptor sets are destroyed automatically when the parent descriptor pool 
	is destroyed.
	*/
}

}   /* namespace jetz */
