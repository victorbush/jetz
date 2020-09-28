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

#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_material.h"
#include "jetz/gpu/vlk/vlk_model.h"
#include "jetz/gpu/vlk/vlk_texture.h"
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
	vlk_pipeline_cache&			pipeline_cache
	)
	: 
	_device(dev),
	_gltf(std::move(gltf)),
	_pipeline_cache(pipeline_cache)
{
	create_buffers();
	create_vertex_input_bindings();
	create_textures();
	create_materials();
	create_pipelines();
}

vlk_model::~vlk_model()
{
	destroy_pipelines();
	destroy_materials();
	destroy_textures();
	destroy_vertex_input_bindings();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void vlk_model::create_buffers()
{
	auto numBuffers = _gltf->buffers.size();
	_buffers.resize(numBuffers);

	// TODO : Sometimes a buffer contains more than indices and vertex data (ie,
	// when textures are embedded into buffers for binary gltf

	/* Create and populate each buffer */
	for (size_t i = 0; i < numBuffers; ++i)
	{
		const auto& b = _gltf->buffers[i];
		size_t buf_size = b.data.size();

		/* Create buffer */
		auto vlk_buf = new vlk_buffer(
			_device,
			buf_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY);
			
		_buffers[i] = uptr<vlk_buffer>(vlk_buf);

		/* Transfer data to GPU */
		_buffers[i]->update((void*)b.data.data(), 0, buf_size);
	}
}

void vlk_model::create_materials()
{
	for (const auto& mat : _gltf->materials)
	{
		load_material(mat);
	}
}

void vlk_model::create_pipelines()
{
	/*
	Create vertex input attribute descriptions.
	Find pipeline for each mesh primitive.
	*/
	size_t numPrimitives = 0;
	_primitives.resize(_gltf->meshes.size());

	for (size_t i = 0; i < _gltf->meshes.size(); ++i)
	{
		const auto& mesh = _gltf->meshes[i];

		for (size_t j = 0; j < mesh.primitives.size(); ++j)
		{
			const auto& prim = mesh.primitives[j];
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			// Function to load a vertex attribute description
			auto loadAttribute = [this, prim, &attributeDescriptions](attribute_type type)
			{
				// Try to find the attribute in this primitve
				auto attr = prim.attributes.find(attribute_names[type]);
				if (attr != prim.attributes.end())
				{
					assert(attribute_locations[type] >= 0);

					const auto& accessor = _gltf->accessors[attr->second];

					VkVertexInputAttributeDescription desc = {};
					desc.binding = accessor.bufferView;
					desc.location = attribute_locations[type];
					desc.format = get_vk_format(accessor.type, accessor.componentType);
					desc.offset = static_cast<uint32_t>(accessor.byteOffset);

					attributeDescriptions.push_back(desc);
				}
			};

			loadAttribute(POSITION);
			loadAttribute(NORMAL);
			//loadAttribute(TANGENT);
			loadAttribute(TEXCOORD_0);
			//loadAttribute(TEXCOORD_1);
			//loadAttribute(COLOR_0);
			//loadAttribute(JOINTS_0);
			//loadAttribute(WEIGHTS_0;

			/* Get a pipeline for this mesh primitive */
			auto create_info = vlk_pipeline_create_info();
			create_info.vertex_attribute_descriptions = attributeDescriptions;
			create_info.vertex_binding_descriptions = _vertex_binding_descriptions;
			const auto& pipeline = _pipeline_cache.create_gltf_pipeline(create_info);

			/* Create a primitive wrapper to store data for this primitive */
			auto p = Primitive(mesh.primitives[j], pipeline);
			p.id = numPrimitives++;
			p.mesh_index = i;
			p.prim_index = j;
			p.material = get_vulkan_material(prim.material);
			_primitives[i].push_back(p);
		}
	}
}

void vlk_model::create_textures()
{
	for (const auto& img : _gltf->images)
	{
		load_texture(img);
	}
}

/**
Creates vertex input bindings
*/
void vlk_model::create_vertex_input_bindings()
{
	std::unordered_map<int, size_t> bindingCache;
	size_t bindingIndex = 0;

	/*
	We only need bindings for buffer views that are used for indices and vertex input
	attributes (position, normal, etc). Buffer view usage is defined by accessor,
	so go through accessors and create bindings for each accessor, if appropriate.

	If a buffer view is only used by one accessor, then that accesor will determine
	the byte stride to use.

	If a buffer view is used by two or more accessors, the buffer view must define
	the byte stride.
	*/
	for (size_t i = 0; i < _gltf->accessors.size(); ++i)
	{
		const auto& a = _gltf->accessors[i];
		const auto& bv = _gltf->bufferViews[a.bufferView];

		/* Check if binding description already created for this buffer view */
		auto bi = bindingCache.find(a.bufferView);
		if (bi != bindingCache.end())
		{
			if (bv.byteStride == 0)
			{
				// TODO : non fatal error
				throw std::runtime_error("Buffer views that are accessed by more than one accessor should define a byte stride.");
			}

			/* Already processed this buffer view */
			continue;
		}

		/* Determine byte stride */
		int byteStride = a.ByteStride(bv);

		if (byteStride < 0)
		{
			// TODO : make non fatal and log error
			throw std::runtime_error("Invalid byte stride.");

			byteStride = 0;
		}

		/* Create new binding */
		_vertex_binding_buffers.resize(bindingIndex + 1);
		_vertex_binding_offsets.resize(bindingIndex + 1);
		_vertex_binding_descriptions.resize(bindingIndex + 1);

		/* Map the buffer view's buffer index to a GPU buffer */
		_vertex_binding_buffers[bindingIndex] = _buffers[bv.buffer]->get_handle();

		/* Record byte offset into buffer */
		_vertex_binding_offsets[bindingIndex] = static_cast<VkDeviceSize>(bv.byteOffset);

		/* Create the vertex binding description */
		_vertex_binding_descriptions[bindingIndex] = {};
		_vertex_binding_descriptions[bindingIndex].binding = static_cast<uint32_t>(bindingIndex);
		_vertex_binding_descriptions[bindingIndex].stride = static_cast<uint32_t>(byteStride);
		_vertex_binding_descriptions[bindingIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		/* Add to the temporary cache so we know this buffer view has been processed */
		bindingCache[a.bufferView] = bindingIndex++;
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

void vlk_model::destroy_pipelines()
{
	/* Let the pipeline cache handle cleanup */
}

void vlk_model::destroy_textures()
{
	for (auto& tex : _textures)
	{
		tex.reset();
	}

	_textures.clear();
}

void vlk_model::destroy_vertex_input_bindings()
{
	/* Nothing to do */
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
	auto material = new vlk_material(_device);

	/*
	Normal/occlusion/emissive
	*/

	/* Alpha mode/cutoff */
	// TODO

	/* Double/sided */
	// TODO

	/* Normal texture */
	auto addAttr = mat.additionalValues.find(MAT_NORMAL_TEXTURE);
	if (addAttr != mat.additionalValues.end())
	{
		auto texIdx = addAttr->second.TextureIndex();
		material->normal_texture = get_vulkan_texture(texIdx);
	}

	/* Emissive factor */
	addAttr = mat.additionalValues.find(MAT_EMISSIVE_FACTOR);
	if (addAttr != mat.additionalValues.end())
	{
		material->emissive_factor = glm::make_vec3(addAttr->second.number_array.data());
	}

	/* Emissive texture */
	addAttr = mat.additionalValues.find(MAT_EMISSIVE_TEXTURE);
	if (addAttr != mat.additionalValues.end())
	{
		auto texIdx = addAttr->second.TextureIndex();
		material->emissive_texture = get_vulkan_texture(texIdx);
	}

	/* Occlusion texture */
	// TODO

	/*
	PBR Metallic/Roughness
	*/

	/* Base color factor */
	auto attr = mat.values.find(MAT_BASE_COLOR_FACTOR);
	if (attr != mat.values.end())
	{
		material->base_color_factor = glm::vec4(glm::make_vec4(attr->second.ColorFactor().data()));
	}

	/* Base color texture */
	attr = mat.values.find(MAT_BASE_COLOR_TEXTURE);
	if (attr != mat.values.end())
	{
		auto texIdx = attr->second.TextureIndex();
		material->base_color_texture = get_vulkan_texture(texIdx);
	}

	/* Metallic factor */
	attr = mat.values.find(MAT_METALLIC_FACTOR);
	if (attr != mat.values.end())
	{
		material->metallic_factor = static_cast<float>(attr->second.Factor());
	}

	/* Roughness factor */
	attr = mat.values.find(MAT_ROUGHNESS_FACTOR);
	if (attr != mat.values.end())
	{
		material->roughness_factor = static_cast<float>(attr->second.Factor());
	}

	/* Metallic/roughness texture */
	attr = mat.values.find(MAT_METALLIC_ROUGHNESS_TEXTURE);
	if (attr != mat.values.end())
	{
		auto texIdx = attr->second.TextureIndex();
		material->metallic_roughness_texture = get_vulkan_texture(texIdx);
	}

	/*
	Save material
	*/
	_materials.push_back(sptr<vlk_material>(material));
}

void vlk_model::load_texture(const tinygltf::Image& image)
{
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

	/* Bind vertex buffers for mesh */
	vkCmdBindVertexBuffers(cmd, 0, static_cast<uint32_t>(_vertex_binding_buffers.size()), _vertex_binding_buffers.data(), _vertex_binding_offsets.data());

	/*
	Render each mesh component (GLTF "primitives")
	*/
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		const auto& prim = _primitives[index][i];

		/*
		Bind pipeline for primitive
		*/
		prim.pipeline.bind(cmd);

		/*
		Set shader push constants
		*/
		push_constant pc = {};
		pc.vertex.model_matrix = transform;

		uint32_t pc_vert_size = sizeof(push_constant_vertex);
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

		vkCmdBindIndexBuffer(cmd, _buffers[bv.buffer]->get_handle(), offset, idx_type);

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
