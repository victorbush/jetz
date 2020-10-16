/*=============================================================================
vlk_model.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <unordered_map>

#include "jetz/main/common.h"
#include "jetz/gpu/gpu_model.h"
#include "thirdparty/tinygltf/tiny_gltf.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs_transform_component;
class gpu_frame;
class vlk_buffer;
class vlk_device;
class vlk_frame;
class vlk_material;
class vlk_texture;
class vlk_gltf_pipeline;
class vlk_pipeline_cache;

/*=============================================================================
CLASS
=============================================================================*/

class vlk_model : public gpu_model {

public:

	vlk_model
		(
		vlk_device&					dev,
		uptr<tinygltf::Model>		gltf,
		sptr<vlk_pipeline_cache>	pipeline_cache
		);

	virtual ~vlk_model() override;

	/*-----------------------------------------------------
	Public Types
	-----------------------------------------------------*/
	enum attribute_type
	{
		POSITION,
		NORMAL,
		TANGENT,
		TEXCOORD_0,
		TEXCOORD_1,
		COLOR_0,
		JOINTS_0,
		WEIGHTS_0
	};

	/*-----------------------------------------------------
	Public Variables
	-----------------------------------------------------*/
	const std::vector<std::string> attribute_names =
	{
		"POSITION",
		"NORMAL",
		"TANGENT",
		"TEXCOORD_0",
		"TEXCOORD_1",
		"COLOR_0",
		"JOINTS_0",
		"WEIGHTS_0"
	};

	// Locations hard coded in shader right now.
	// -1 means not used.
	const std::vector<int> attribute_locations =
	{
		0,		// POSITION
		1,		// NORMAL
		-1,		// TANGENT
		2,		// TEXCOORD_0
		-1,		// TEXCOORD_1
		-1,		// COLOR_0
		-1,		// JOINTS_0
		-1		// WEIGHTS_0
	};

	/* Material attributes */
	const char* MAT_PBR_METALLIC_ROUGHNESS = "pbrMetallicRoughness";
	const char* MAT_ALPHA_CUTOFF = "alphaCutoff";
	const char* MAT_ALPHA_MODE = "alphaMode";
	const char* MAT_DOUBLE_SIDED = "doubleSided";
	const char* MAT_EMISSIVE_FACTOR = "emissiveFactor";
	const char* MAT_EMISSIVE_TEXTURE = "emissiveTexture";
	const char* MAT_NORMAL_TEXTURE = "normalTexture";
	const char* MAT_OCCLUSION_TEXTURE = "occlusionTexture";

	/* Material - PBR */
	const char* MAT_BASE_COLOR_FACTOR = "baseColorFactor";
	const char* MAT_BASE_COLOR_TEXTURE = "baseColorTexture";
	const char* MAT_METALLIC_FACTOR = "metallicFactor";
	const char* MAT_ROUGHNESS_FACTOR = "roughnessFactor";
	const char* MAT_METALLIC_ROUGHNESS_TEXTURE = "metallicRoughnessTexture";

	/*-----------------------------------------------------
	jetz::gpu_model Methods
	-----------------------------------------------------*/

	virtual void render(const gpu_frame& frame, const ecs_transform_component& transform) override;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:
	/*-----------------------------------------------------
	Class to encapsulate GLTF mesh primitives
	-----------------------------------------------------*/

	class Primitive
	{
	public:
		const tinygltf::Primitive&	data;		/* the actual primitive data from the GLTF model */
		const vlk_gltf_pipeline&	pipeline;	/* pipeline used to render this mesh primitive */
		wptr<vlk_material>			material;
		VkIndexType					index_type;
		std::vector<VkBuffer>		input_binding_buffers;
		std::vector<VkDeviceSize>	input_binding_offsets;

		Primitive(const tinygltf::Primitive& data, const vlk_gltf_pipeline& pipeline)
			: data(data), pipeline(pipeline), index_type(VK_INDEX_TYPE_UINT16) {}
	};

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_buffer(int bufferIndex);
	void create_materials();
	void create_primitives();
	void create_textures();
	void destroy_buffers();
	void destroy_materials();
	void destroy_primitives();
	void destroy_textures();

	VkFormat					get_vk_format(int gltfType, int gltfComponentType) const;
	VkIndexType					get_vk_index_type(int gltfComponentType) const;
	wptr<vlk_material>			get_vulkan_material(int index);
	wptr<vlk_texture>			get_vulkan_texture(int index);
	void						load_material(const tinygltf::Material& mat);
	void						load_primitive(const tinygltf::Primitive& prim, uint32_t mesh_idx);
	void						load_texture(const tinygltf::Image& image);

	void render_mesh
		(
		size_t							index,
		const vlk_frame&				frame,
		VkCommandBuffer					cmd,
		glm::mat4						transform
		) const;

	void render_node
		(
		size_t							index,
		const vlk_frame&				frame,
		VkCommandBuffer					cmd,
		glm::mat4						parent_transform
		) const;

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&							_device;
	uptr<tinygltf::Model>				_gltf;
	sptr<vlk_pipeline_cache>			_pipeline_cache;

	/*
	Create/destroy
	*/
	std::unordered_map<int, uptr<vlk_buffer>>	_buffers;	/* Dictionary of buffers keyed by buffer index (from the gltf file) */
	std::vector<sptr<vlk_material>>		_materials;
	std::vector<sptr<vlk_texture>>		_textures;

	/* A list of primitives for each mesh */
	std::vector<std::vector<Primitive>>	_primitives;
};

}   /* namespace jetz */
