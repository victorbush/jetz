/*=============================================================================
vlk_imgui_pipeline.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/pipelines/vlk_imgui_pipeline.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_imgui_pipeline::vlk_imgui_pipeline
	(
	vlk_device&						device,
	VkRenderPass					render_pass,
	VkExtent2D						extent
	)
	: vlk_pipeline(device, render_pass, extent)
{
	create_buffers();
	create_font_texture();
	create_descriptor_pool();
	create_descriptor_layout();
	create_descriptor_sets();
	create_pipeline_layout();
	create_pipeline();
}

vlk_imgui_pipeline::~vlk_imgui_pipeline()
{
	destroy_pipeline();
	destroy_pipeline_layout();
	destroy_descriptor_sets();
	destroy_descriptor_layout();
	destroy_descriptor_pool();
	destroy_font_texture();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_imgui_pipeline::render(const vlk_frame& frame, ImDrawData* draw_data)
{
	if (!draw_data)
		return;

	/*
	Update vertex and index buffers
	*/
	uint32_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
	uint32_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	if (vertex_size == 0 || index_size == 0)
		return;

	if (vertex_buffers[frame.image_idx] == nullptr
		|| vertex_buffer_sizes[frame.image_idx] < vertex_size)
	{
		if (vertex_buffers[frame.image_idx] != nullptr)
		{
			delete vertex_buffers[frame.image_idx];
		}

		vertex_buffers[frame.image_idx] = new jetz::vlk_buffer(dev, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		vertex_buffer_sizes[frame.image_idx] = vertex_size;
	}

	if (index_buffers[frame.image_idx] == nullptr
		|| index_buffer_sizes[frame.image_idx] < index_size)
	{
		if (index_buffers[frame.image_idx] != nullptr)
		{
			delete index_buffers[frame.image_idx];
		}

		index_buffers[frame.image_idx] = new jetz::vlk_buffer(dev, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		index_buffer_sizes[frame.image_idx] = index_size;
	}

	// Upload Vertex and index Data:
	{
		ImDrawVert* vertex_dest = NULL;
		ImDrawIdx* index_dest = NULL;

		VmaAllocation vertex_alloc = vertex_buffers[frame.image_idx]->get_allocation();
		VmaAllocation index_alloc = index_buffers[frame.image_idx]->get_allocation();

		VkResult result = vmaMapMemory(dev.get_allocator(), vertex_alloc, (void**)&vertex_dest);
		if (result != VK_SUCCESS)
		{
			LOG_FATAL("Error");
		}

		result = vmaMapMemory(dev.get_allocator(), index_alloc, (void**)&index_dest);
		if (result != VK_SUCCESS)
		{
			LOG_FATAL("Error");
		}

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			memcpy(vertex_dest, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(index_dest, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vertex_dest += cmd_list->VtxBuffer.Size;
			index_dest += cmd_list->IdxBuffer.Size;
		}

		vmaUnmapMemory(dev.get_allocator(), vertex_alloc);
		vmaUnmapMemory(dev.get_allocator(), index_alloc);
	}

	/*
	Bind pipeline and descriptor sets
	*/
	vkCmdBindPipeline(frame.cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
	vkCmdBindDescriptorSets(frame.cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptor_sets[frame.image_idx], 0, NULL);

	/*
	Bind buffers
	*/
	VkBuffer vert_buffers[1] = { vertex_buffers[frame.image_idx]->get_handle() };
	VkDeviceSize vert_offset[1] = { 0 };
	vkCmdBindVertexBuffers(frame.cmd_buf, 0, 1, vert_buffers, vert_offset);
	vkCmdBindIndexBuffer(frame.cmd_buf, index_buffers[frame.image_idx]->get_handle(), 0, VK_INDEX_TYPE_UINT16);

	/*
	Setup viewport
	*/
	draw_data->DisplaySize.x = (float)extent.width;
	draw_data->DisplaySize.y = (float)extent.height;

	VkViewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = draw_data->DisplaySize.x;
	viewport.height = draw_data->DisplaySize.y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(frame.cmd_buf, 0, 1, &viewport);

	// Setup scale and translation:
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
	{
		float scale[2];
		scale[0] = 2.0f / draw_data->DisplaySize.x;
		scale[1] = 2.0f / draw_data->DisplaySize.y;
		float translate[2];
		translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
		translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
		vkCmdPushConstants(frame.cmd_buf, layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
		vkCmdPushConstants(frame.cmd_buf, layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);
	}

	// Render the command lists:
	int vtx_offset = 0;
	int idx_offset = 0;
	ImVec2 display_pos = draw_data->DisplayPos;

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer.Data[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Apply scissor/clipping rectangle
				// FIXME: We could clamp width/height based on clamped min/max values.
				VkRect2D scissor;
				scissor.offset.x = (int32_t)(pcmd->ClipRect.x - display_pos.x) > 0 ? (int32_t)(pcmd->ClipRect.x - display_pos.x) : 0;
				scissor.offset.y = (int32_t)(pcmd->ClipRect.y - display_pos.y) > 0 ? (int32_t)(pcmd->ClipRect.y - display_pos.y) : 0;
				scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
				scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?
				vkCmdSetScissor(frame.cmd_buf, 0, 1, &scissor);

				// Draw
				vkCmdDrawIndexed(frame.cmd_buf, pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_imgui_pipeline::create_buffers()
{
	/*
	Resize buffer arrays. Buffers are created and updated as needed at render time.
	*/
	index_buffers.resize(gpu::num_frame_buf, nullptr);
	index_buffer_sizes.resize(gpu::num_frame_buf, 0);
	vertex_buffers.resize(gpu::num_frame_buf, nullptr);
	vertex_buffer_sizes.resize(gpu::num_frame_buf, 0);
}

void vlk_imgui_pipeline::create_descriptor_layout()
{
	/* Font texture */
	VkDescriptorSetLayoutBinding font_texture_binding = {};
	font_texture_binding.binding = 0;
	font_texture_binding.descriptorCount = 1;
	font_texture_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	font_texture_binding.pImmutableSamplers = NULL;
	font_texture_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings =
	{
		font_texture_binding
	};

	VkDescriptorSetLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = (uint32_t)bindings.size();
	layout_info.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(dev.get_handle(), &layout_info, NULL, &descriptor_layout) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create descriptor set layout.");
	}
}

void vlk_imgui_pipeline::create_descriptor_pool()
{
	// TODO ???

	std::vector<VkDescriptorPoolSize> pool_sizes(1);
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[0].descriptorCount = gpu::num_frame_buf;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
	pool_info.pPoolSizes = pool_sizes.data();

	// TOOD : what should maxSets be??
	pool_info.maxSets = gpu::num_frame_buf;

	if (vkCreateDescriptorPool(dev.get_handle(), &pool_info, NULL, &descriptor_pool) != VK_SUCCESS) 
	{
		LOG_FATAL("Failed to create descriptor pool.");
	}
}

void vlk_imgui_pipeline::create_descriptor_sets()
{
	std::vector<VkDescriptorSetLayout> layouts(gpu::num_frame_buf, descriptor_layout);
	descriptor_sets.resize(gpu::num_frame_buf);

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = descriptor_pool;
	alloc_info.descriptorSetCount = (uint32_t)descriptor_sets.size();
	alloc_info.pSetLayouts = layouts.data();

	VkResult result = vkAllocateDescriptorSets(dev.get_handle(), &alloc_info, descriptor_sets.data());
	if (result != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate descriptor sets.");
	}

	for (int i = 0; i < gpu::num_frame_buf; ++i)
	{
		VkWriteDescriptorSet descriptor_writes[1];
		size_t write_idx = 0;

		/* Font texture */
		VkDescriptorImageInfo image_info = {};
		image_info.imageLayout = font_texture->get_image_info().imageLayout;
		image_info.imageView = font_texture->get_image_info().imageView;
		image_info.sampler = font_texture_sampler;

		descriptor_writes[write_idx] = {};
		descriptor_writes[write_idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptor_writes[write_idx].dstSet = descriptor_sets[i];
		descriptor_writes[write_idx].dstBinding = 0;
		descriptor_writes[write_idx].dstArrayElement = 0;
		descriptor_writes[write_idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor_writes[write_idx].descriptorCount = 1;
		descriptor_writes[write_idx].pImageInfo = &image_info;
		++write_idx;

		vkUpdateDescriptorSets(dev.get_handle(), write_idx, descriptor_writes, 0, NULL);
	}
}

void vlk_imgui_pipeline::create_font_texture()
{
	ImGuiIO& io = ImGui::GetIO();

	// TODO : make sure fonts exist, fall back to default if doesn't - imgui will assert otherwise.
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Cousine-Regular.ttf").data(), 12);
	ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 13, NULL, NULL);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Karla-Regular.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/ProggyClean.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/ProggyTiny.ttf").data(), 12);
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetFilePath("Fonts/Roboto-Medium.ttf").data(), 13);

	/* 
	Build and load the texture atlas into a texture
	(In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
	*/
	int width, height;
	unsigned char* pixels = NULL;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, NULL);

	/*
	At this point you've got the texture data and you need to upload that your your graphic system:
	After we have created the texture, store its pointer/identifier (_in whichever format your engine uses_) in 'io.Fonts->TexID'.
	This will be passed back to your via the renderer. Basically ImTextureID == void*. Read FAQ below for details about ImTextureID.
	*/
	vlk_texture_create_info create_info = {};
	create_info.data = (void*)pixels;
	create_info.width = width;
	create_info.height = height;
	create_info.size = (size_t)width * height * 4;
	//create_info.type = TEXTURE_TYPE_2D;

	font_texture = new jetz::vlk_texture(dev, create_info);
	io.Fonts->TexID = (void*)font_texture->get_image();

	/*
	Create sampler
	*/
	VkSamplerCreateInfo sampler_info = {};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;

	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	sampler_info.maxAnisotropy = 1.0f;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = -1000;
	sampler_info.maxLod = 1000;

	if (vkCreateSampler(dev.get_handle(), &sampler_info, NULL, &font_texture_sampler) != VK_SUCCESS) 
	{
		LOG_FATAL("Failed to create texture sampler.");
	}
}

void vlk_imgui_pipeline::create_pipeline()
{
	VkShaderModule vert_shader = dev.create_shader("bin/shaders/imgui.vert.spv");
	VkShaderModule frag_shader = dev.create_shader("bin/shaders/imgui.frag.spv");

	/*
	Shader stage creation
	*/
	VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader;
	vert_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader;
	frag_shader_stage_info.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vert_shader_stage_info, frag_shader_stage_info };

	/*
	Vertex input
	*/

	/* Binding for vertices */
	VkVertexInputBindingDescription vertex_binding = {};
	vertex_binding.binding = 0;
	vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertex_binding.stride = sizeof(ImDrawVert);

	/* Position attribute */
	VkVertexInputAttributeDescription pos_attr = {};
	pos_attr.binding = vertex_binding.binding;
	pos_attr.format = VK_FORMAT_R32G32_SFLOAT;
	pos_attr.location = 0;
	pos_attr.offset = offsetof(ImDrawVert, pos);

	/* Tex-coord attribute */
	VkVertexInputAttributeDescription tex_coord_attr = {};
	tex_coord_attr.binding = vertex_binding.binding;
	tex_coord_attr.format = VK_FORMAT_R32G32_SFLOAT;
	tex_coord_attr.location = 1;
	tex_coord_attr.offset = offsetof(ImDrawVert, uv);

	/* Color attribute */
	VkVertexInputAttributeDescription color_attr = {};
	color_attr.binding = vertex_binding.binding;
	color_attr.format = VK_FORMAT_R8G8B8A8_UNORM;
	color_attr.location = 2;
	color_attr.offset = offsetof(ImDrawVert, col);

	VkVertexInputBindingDescription binding_descriptions[] = { vertex_binding };
	VkVertexInputAttributeDescription attribute_descriptions[] = { pos_attr, tex_coord_attr, color_attr };

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = cnt_of_array(binding_descriptions);
	vertex_input_info.vertexAttributeDescriptionCount = cnt_of_array(attribute_descriptions);
	vertex_input_info.pVertexBindingDescriptions = binding_descriptions;
	vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

	/*
	Input assembly
	*/
	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	/*
	Viewport and scissor
	*/
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewport_state = {};
	viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports = &viewport;
	viewport_state.scissorCount = 1;
	viewport_state.pScissors = &scissor;

	/*
	Rasterizer
	*/
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE; // No cull mode for imgui
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	/*
	Multisampling
	*/
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = NULL; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	/*
	Color blending
	*/
	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f; // Optional
	color_blending.blendConstants[1] = 0.0f; // Optional
	color_blending.blendConstants[2] = 0.0f; // Optional
	color_blending.blendConstants[3] = 0.0f; // Optional

	/*
	Depth/stencil
	*/
	VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
	depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	/*
	Dynamic state
	*/
	VkDynamicState dynamic_states[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = cnt_of_array(dynamic_states);
	dynamic_state.pDynamicStates = dynamic_states;

	/*
	Pipeline
	*/
	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shaderStages;

	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_state;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multisampling;
	pipeline_info.pDepthStencilState = &depth_stencil; // Optional
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = &dynamic_state; // Optional

	pipeline_info.layout = layout;
	pipeline_info.renderPass = render_pass;
	pipeline_info.subpass = 0;

	pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_info.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(dev.get_handle(), VK_NULL_HANDLE, 1, &pipeline_info, NULL, &handle) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create pipeline.");
	}

	/*
	* Cleanup
	*/
	dev.destroy_shader(vert_shader);
	dev.destroy_shader(frag_shader);
}

void vlk_imgui_pipeline::create_pipeline_layout()
{
	std::vector<VkDescriptorSetLayout> set_layouts(1, descriptor_layout);

	/*
	Push constants

	Using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix.
	*/
	VkPushConstantRange push_constants[1];
	memset(push_constants, 0, sizeof(push_constants));
	push_constants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constants[0].offset = sizeof(float) * 0;
	push_constants[0].size = sizeof(float) * 4;

	/*
	Create the pipeline layout
	*/
	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = (uint32_t)set_layouts.size();
	pipeline_layout_info.pSetLayouts = set_layouts.data();
	pipeline_layout_info.pushConstantRangeCount = cnt_of_array(push_constants);
	pipeline_layout_info.pPushConstantRanges = push_constants;

	if (vkCreatePipelineLayout(dev.get_handle(), &pipeline_layout_info, NULL, &layout) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create pipeline layout.");
	}
}

void vlk_imgui_pipeline::destroy_buffers()
{
	for (uint32_t i = 0; i < vertex_buffers.size(); ++i)
	{
		if (vertex_buffers[i] != nullptr)
		{
			delete vertex_buffers[i];
			vertex_buffers[i] = nullptr;
		}

		if (index_buffers[i] != nullptr)
		{
			delete index_buffers[i];
			index_buffers[i] = nullptr;
		}
	}
}

void vlk_imgui_pipeline::destroy_descriptor_layout()
{
	vkDestroyDescriptorSetLayout(dev.get_handle(), descriptor_layout, NULL);
}

void vlk_imgui_pipeline::destroy_descriptor_pool()
{
	vkDestroyDescriptorPool(dev.get_handle(), descriptor_pool, NULL);
}

void vlk_imgui_pipeline::destroy_descriptor_sets()
{
	/*
	Descriptor sets are destroyed automatically when the parent descriptor pool
	is destroyed.
	*/
}

void vlk_imgui_pipeline::destroy_font_texture()
{
	vkDestroySampler(dev.get_handle(), font_texture_sampler, NULL);
	delete font_texture;
}

void vlk_imgui_pipeline::destroy_pipeline()
{
	vkDestroyPipeline(dev.get_handle(), handle, NULL);
}

void vlk_imgui_pipeline::destroy_pipeline_layout()
{
	vkDestroyPipelineLayout(dev.get_handle(), layout, NULL);
}

}   /* namespace jetz */
