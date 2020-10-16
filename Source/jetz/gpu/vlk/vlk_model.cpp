/*=============================================================================
vlk_texture.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <unordered_set>

#include "jetz/ecs/components/ecs_transform_component.h"
#include "jetz/gpu/gpu_frame.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_material.h"
#include "jetz/gpu/vlk/vlk_model.h"
#include "jetz/gpu/vlk/vlk_texture.h"
#include "jetz/gpu/vlk/pipelines/vlk_gltf_pipeline.h"
#include "jetz/gpu/vlk/pipelines/vlk_pipeline_cache.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_model::vlk_model
	(
	vlk_device&					dev,
	uptr<tinygltf::Model>		gltf,
	sptr<vlk_pipeline_cache>	pipeline_cache
	)
	: 
	_device(dev),
	_gltf(std::move(gltf)),
	_pipeline_cache(pipeline_cache)
{
	create_textures();
	create_materials();
	create_primitives();
}

vlk_model::~vlk_model()
{
	destroy_primitives();
	destroy_materials();
	destroy_textures();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_model::render(const gpu_frame& gpu_frame, const ecs_transform_component& transform)
{
	vlk_frame& frame = _device.get_frame(gpu_frame);

	for (const auto& scene : _gltf->scenes)
	{
		for (auto nodeIndex : scene.nodes)
		{
			// TODO : model scale and rotation
			auto root_transform = glm::mat4(1.0f);
			root_transform = glm::translate(root_transform, transform.pos);
			render_node(nodeIndex, frame, frame.cmd_buf, root_transform);
		}
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_model::create_buffer(int bufferIndex)
{
	/* Validate buffer index */
	if (bufferIndex < 0)
	{
		/* No buffer */
		return;
	}

	if (bufferIndex >= _gltf->buffers.size())
	{
		LOG_ERROR_FMT("Invalid buffer index {0}, no buffer created.", bufferIndex);
		return;
	}

	/* Check if buffer already created */
	const auto it = _buffers.find(bufferIndex);
	if (it != _buffers.end())
	{
		/* Buffer already created */
		return;
	}

	/* Create the buffer */
	const auto& b = _gltf->buffers[bufferIndex];
	size_t buf_size = b.data.size();

	auto vlk_buf = new vlk_buffer(
		_device,
		buf_size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	_buffers[bufferIndex] = uptr<vlk_buffer>(vlk_buf);

	/* Transfer data to GPU */
	_buffers[bufferIndex]->update((void*)b.data.data(), 0, buf_size);
}

void vlk_model::create_materials()
{
	for (const auto& mat : _gltf->materials)
	{
		load_material(mat);
	}
}

void vlk_model::create_primitives()
{
	size_t numPrimitives = 0;
	_primitives.resize(_gltf->meshes.size());

	for (size_t i = 0; i < _gltf->meshes.size(); ++i)
	{
		const auto& mesh = _gltf->meshes[i];

		for (size_t j = 0; j < mesh.primitives.size(); ++j)
		{
			const auto& prim = mesh.primitives[j];
			load_primitive(prim, i);
		}
	}
}

void vlk_model::load_primitive(const tinygltf::Primitive& prim, uint32_t mesh_idx)
{
	std::vector<VkVertexInputAttributeDescription>	input_attributes;
	std::vector<VkVertexInputBindingDescription>	input_bindings;
	std::vector<VkBuffer>							input_binding_buffers;
	std::vector<VkDeviceSize>						input_binding_offsets;

	std::unordered_map<int, int> accessor_index_to_input_binding;

	/* Function to load a vertex attribute description */
	auto load_attribute = 
		[this, prim, &accessor_index_to_input_binding, &input_attributes, &input_bindings, &input_binding_buffers, &input_binding_offsets]
		(attribute_type type)
	{
		/* Try to find the attribute in this primitve */
		auto attr = prim.attributes.find(attribute_names[type]);
		if (attr == prim.attributes.end())
		{
			/* Not found */
			return;
		}

		/* Get accessor and buffer view */
		const auto& a = _gltf->accessors[attr->second];
		const auto& bv = _gltf->bufferViews[a.bufferView];

		/* Load buffer if needed */
		create_buffer(bv.buffer);

		/* Create input binding if needed - we create an input binding for each accessor */
		auto accessorIndex = attr->second;
		if (accessor_index_to_input_binding.find(accessorIndex) == accessor_index_to_input_binding.end())
		{
			/* Determine byte stride */
			int byteStride = a.ByteStride(bv);
			if (byteStride < 0)
			{
				LOG_ERROR_FMT("Invalid byte stride {0}; setting to 0.", byteStride);
				LOG_ERROR_FMT("Accessor: {0}; BufferView: {1}", a.name, bv.name);

				byteStride = 0;
			}

			/* Determine binding index */
			auto bindingIndex = input_bindings.size();

			/* Determine binding buffer */
			input_binding_buffers.push_back(_buffers[bv.buffer]->get_handle());

			/* Determine binding offset (within the buffer) */
			input_binding_offsets.push_back(bv.byteOffset + a.byteOffset);

			/* Create input binding */
			VkVertexInputBindingDescription input_binding = {};
			input_binding.binding = bindingIndex;
			input_binding.stride = static_cast<uint32_t>(byteStride);
			input_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			input_bindings.push_back(input_binding);

			accessor_index_to_input_binding[accessorIndex] = bindingIndex;
		}

		/* Get input binding index */
		auto bindingIndex = accessor_index_to_input_binding[accessorIndex];

		/* Create the attribute description */
		assert(attribute_locations[type] >= 0);

		VkVertexInputAttributeDescription input_attribute = {};
		input_attribute.binding = bindingIndex;
		input_attribute.location = attribute_locations[type];
		input_attribute.format = get_vk_format(a.type, a.componentType);
		input_attribute.offset = 0;

		input_attributes.push_back(input_attribute);
	};

	/* Load attributes */
	load_attribute(POSITION);
	load_attribute(NORMAL);
	//load_attribute(TANGENT);
	load_attribute(TEXCOORD_0);
	//load_attribute(TEXCOORD_1);
	//load_attribute(COLOR_0);
	//load_attribute(JOINTS_0);
	//load_attribute(WEIGHTS_0;

	/* Create index buffer */
	const auto& indicesAccessor = _gltf->accessors[prim.indices];
	const auto& indicesBufferView = _gltf->bufferViews[indicesAccessor.bufferView];
	create_buffer(indicesBufferView.buffer);

	/* Get a pipeline for this mesh primitive */
	auto create_info = vlk_pipeline_create_info();
	create_info.vertex_attribute_descriptions = input_attributes;
	create_info.vertex_binding_descriptions = input_bindings;
	const auto& pipeline = _pipeline_cache->create_gltf_pipeline(create_info);

	/* Create a primitive wrapper to store data for this primitive */
	auto p = Primitive(prim, pipeline);
	p.material = get_vulkan_material(prim.material);
	p.input_binding_buffers = input_binding_buffers;
	p.input_binding_offsets = input_binding_offsets;
	_primitives[mesh_idx].push_back(p);
}

void vlk_model::create_textures()
{
	for (const auto& img : _gltf->images)
	{
		load_texture(img);
	}
}

void vlk_model::destroy_buffers()
{
	for (size_t i = 0; i < _buffers.size(); ++i)
	{
		_buffers[i].reset();
	}
}

void vlk_model::destroy_materials()
{
	for (auto& mat : _materials)
	{
		mat.reset();
	}

	_materials.clear();
}

void vlk_model::destroy_primitives()
{
	/* Let the destructors handle cleanup */
	_primitives.clear();

	/* The pipeline cache will handle pipeline cleanup */
}

void vlk_model::destroy_textures()
{
	for (auto& tex : _textures)
	{
		tex.reset();
	}

	_textures.clear();
}

/**
Gets a Vulkan format from a GLTF type and format.

gltfType - TINYGLTF_TYPE_*
gltfComponentType - TINYGLTF_COMPONENT_TYPE_*
*/
VkFormat vlk_model::get_vk_format(int gltfType, int gltfComponentType) const
{
	if (gltfType == TINYGLTF_TYPE_VEC2)
	{
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_BYTE)
			return VK_FORMAT_R8G8_SINT;
		//if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			return VK_FORMAT_R32G32_SFLOAT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_INT)
			return VK_FORMAT_R32G32_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_SHORT)
			return VK_FORMAT_R16G16_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			return VK_FORMAT_R8G8_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			return VK_FORMAT_R32G32_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			return VK_FORMAT_R16G16_UINT;
	}
	else if (gltfType == TINYGLTF_TYPE_VEC3)
	{
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_BYTE)
			return VK_FORMAT_R8G8B8_SINT;
		//if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			return VK_FORMAT_R32G32B32_SFLOAT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_INT)
			return VK_FORMAT_R32G32B32_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_SHORT)
			return VK_FORMAT_R16G16B16_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			return VK_FORMAT_R8G8B8_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			return VK_FORMAT_R32G32B32_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			return VK_FORMAT_R16G16B16_UINT;
	}
	else if (gltfType == TINYGLTF_TYPE_VEC4)
	{
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_BYTE)
			return VK_FORMAT_R8G8B8A8_SINT;
		//if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_INT)
			return VK_FORMAT_R32G32B32A32_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_SHORT)
			return VK_FORMAT_R16G16B16A16_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			return VK_FORMAT_R8G8B8A8_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			return VK_FORMAT_R32G32B32A32_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			return VK_FORMAT_R16G16B16A16_UINT;
	}
	else if (gltfType == TINYGLTF_TYPE_SCALAR)
	{
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_BYTE)
			return VK_FORMAT_R8_SINT;
		//if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			return VK_FORMAT_R32_SFLOAT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_INT)
			return VK_FORMAT_R32_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_SHORT)
			return VK_FORMAT_R16_SINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
			return VK_FORMAT_R8_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			return VK_FORMAT_R32_UINT;
		if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			return VK_FORMAT_R16_UINT;
	}
	//else if (gltfType == TINYGLTF_TYPE_MAT2)
	//else if (gltfType == TINYGLTF_TYPE_MAT3)
	//else if (gltfType == TINYGLTF_TYPE_MAT4)
	//else if (gltfType == TINYGLTF_TYPE_MATRIX)
	//else if (gltfType == TINYGLTF_TYPE_VECTOR)

	LOG_FATAL("Unknown format");
}

/**
Gets a Vulkan format from a GLTF type and format.

gltfComponentType - TINYGLTF_COMPONENT_TYPE_*
*/
VkIndexType vlk_model::get_vk_index_type(int gltfComponentType) const
{
	if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
		return VK_INDEX_TYPE_UINT32;
	if (gltfComponentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
		return VK_INDEX_TYPE_UINT16;

	LOG_FATAL("Unknown index type.");
}

wptr<vlk_material> vlk_model::get_vulkan_material(int index)
{
	if (index < 0 || index >= _materials.size())
	{
		LOG_ERROR("Invalid material index.");
		return wptr<vlk_material>();
	}

	return _materials[index];
}

wptr<vlk_texture> vlk_model::get_vulkan_texture(int index)
{
	if (index < 0 || index >= _gltf->textures.size())
	{
		LOG_ERROR("Invalid texture index.");
		return wptr<vlk_texture>();
	}

	auto imgIdx = _gltf->textures[index].source;

	if (imgIdx < 0 || imgIdx >= _textures.size())
	{
		LOG_ERROR("Invalid image index.");
		return wptr<vlk_texture>();
	}
	
	return _textures[imgIdx];
}

void vlk_model::load_material(const tinygltf::Material& mat)
{
	vlk_material_create_info mat_info = {};
	int tex_idx;

	/*
	Normal/occlusion/emissive
	*/

	/* Alpha mode/cutoff */
	// TODO

	/* Double/sided */
	// TODO

	/* Normal texture */
	tex_idx = mat.normalTexture.index;
	if (tex_idx >= 0)
	{
		mat_info.normal_texture = get_vulkan_texture(tex_idx);
	}

	/* Emissive factor */
	mat_info.emissive_factor = glm::make_vec3(mat.emissiveFactor.data());

	/* Emissive texture */
	tex_idx = mat.emissiveTexture.index;
	if (tex_idx >= 0)
	{
		mat_info.emissive_texture = get_vulkan_texture(tex_idx);
	}

	/* Occlusion texture */
	tex_idx = mat.occlusionTexture.index;
	if (tex_idx >= 0)
	{
		mat_info.occlusion_texture = get_vulkan_texture(tex_idx);
	}

	/*
	PBR Metallic/Roughness
	*/

	/* Base color factor */
	mat_info.base_color_factor = glm::make_vec4(mat.pbrMetallicRoughness.baseColorFactor.data());

	/* Base color texture */
	tex_idx = mat.pbrMetallicRoughness.baseColorTexture.index;
	if (tex_idx >= 0)
	{
		mat_info.base_color_texture = get_vulkan_texture(tex_idx);
	}

	/* Metallic factor */
	mat_info.metallic_factor = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);

	/* Roughness factor */
	mat_info.roughness_factor = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);

	/* Metallic/roughness texture */
	tex_idx = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
	if (tex_idx >= 0)
	{
		mat_info.metallic_roughness_texture = get_vulkan_texture(tex_idx);
	}

	/*
	Save material
	*/
	auto material = new vlk_material(_device, mat_info);
	_materials.push_back(sptr<vlk_material>(material));
}

void vlk_model::load_texture(const tinygltf::Image& image)
{
	/* Setup */
	vlk_texture_create_info create_info = {};
	create_info.data = (void*)image.image.data();
	create_info.size = image.image.size();
	create_info.width = static_cast<uint32_t>(image.width);
	create_info.height = static_cast<uint32_t>(image.height);

	/* Create and store texture */
	auto texture = new vlk_texture(_device, create_info);
	_textures.push_back(sptr<vlk_texture>(texture));
}

void vlk_model::render_mesh
	(
	size_t							index,
	const vlk_frame&				frame,
	VkCommandBuffer					cmd,
	glm::mat4						transform
	) const
{
	/* Validate index */
	if (index >= _gltf->meshes.size())
	{
		/* Invalid index */
		return;
	}

	/* Get the mesh */
	const auto& mesh = _gltf->meshes[index];

	/*
	Render each mesh component (GLTF "primitives")
	*/
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		const auto& prim = _primitives[index][i];

		/* Bind vertex buffers for primitive */
		vkCmdBindVertexBuffers(cmd, 0, static_cast<uint32_t>(prim.input_binding_buffers.size()), prim.input_binding_buffers.data(), prim.input_binding_offsets.data());

		/*
		Bind pipeline for primitive
		*/
		prim.pipeline.bind(cmd);

		/*
		Set shader push constants
		*/
		vlk_gltf_push_constant pc = {};
		pc.vertex.model_matrix = transform;

		uint32_t pc_vert_size = sizeof(vlk_gltf_push_constant_vertex);
		vkCmdPushConstants(frame.cmd_buf, prim.pipeline.get_layout_handle(), VK_SHADER_STAGE_VERTEX_BIT, 0, pc_vert_size, &pc.vertex);

		/*
		Bind material
		*/
		auto mat = prim.material.lock();
		if (mat == nullptr)
		{
			LOG_ERROR("Could not get material for mesh.");
		}
		else
		{
			mat->bind(frame, prim.pipeline.get_layout_handle());
		}

		/*
		Bind index buffer for primitive
		*/
		auto a = _gltf->accessors[prim.data.indices];
		auto bv = _gltf->bufferViews[a.bufferView];
		auto offset = a.byteOffset + bv.byteOffset;
		auto idx_type = get_vk_index_type(a.componentType);
		auto buffer = _buffers.at(bv.buffer)->get_handle();

		vkCmdBindIndexBuffer(cmd, buffer, offset, idx_type);

		/*
		Draw indexed
		*/
		vkCmdDrawIndexed(cmd, static_cast<uint32_t>(a.count), 1, 0, 0, 0);
	}
}

void vlk_model::render_node
	(
	size_t							index,
	const vlk_frame&				frame,
	VkCommandBuffer					cmd,
	glm::mat4						parent_transform
	) const
{
	/* Validate index */
	// TOOD : the GLTF loader should be trusted to provide valid indices
	if (index >= _gltf->nodes.size())
	{
		/* Invalid index */
		LOG_WARN_FMT("Invalid GLTF node index {0}.", index);
		return;
	}

	/* Get the node */
	const auto& node = _gltf->nodes[index];

	/*
	Compute the transform for this node
	*/
	glm::mat4 transform = glm::mat4(parent_transform);

	if (node.matrix.size() == 16)
	{
		// Node uses a transformation matrix
		auto matrix = glm::make_mat4x4(node.matrix.data());
		transform = transform * glm::mat4(matrix);
	}
	else
	{
		// Node either uses translation, rotation, and scale vectors, or has no transform

		/* Scale */
		if (node.scale.size() == 3)
		{
			// TODO : potentially make tinygltf allow float vectors/matrices so don't have to convert all the time.
			// either that or convert and store manually.
			auto scale = glm::vec3(glm::make_vec3(node.scale.data()));
			transform = glm::scale(transform, scale);
		}

		/* Rotation */
		if (node.rotation.size() == 4)
		{
			auto rotateQuat = glm::highp_fquat(glm::make_quat(node.rotation.data()));
			transform = transform * glm::mat4_cast(rotateQuat);
		}

		/* Translation */
		if (node.translation.size() == 3)
		{
			auto translation = glm::vec3(glm::make_vec3(node.translation.data()));
			transform = glm::translate(transform, translation);
		}
	}

	/*
	A node can contain a mesh or a camera, or it can be empty and just define a transform
	*/
	if (node.mesh >= 0 && node.mesh < _gltf->meshes.size())
	{
		/* Node points to mesh */
		render_mesh(node.mesh, frame, cmd, transform);
	}
	else if (node.camera >= 0 && node.camera < _gltf->cameras.size())
	{
		/* Node points to camera */
	}
	else
	{
		/* Transform only */
	}

	/* Render child nodes */
	for (auto childIndex : node.children)
	{
		render_node(childIndex, frame, cmd, transform);
	}
}

}   /* namespace jetz */
