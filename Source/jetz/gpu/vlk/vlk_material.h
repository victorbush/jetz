/*=============================================================================
vlk_material.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "jetz/gpu/gpu_material.h"
#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/gpu/vlk/vlk_texture.h"
#include "jetz/main/common.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

/**
This is used to initialize a material.
*/
struct vlk_material_create_info
{
	/*
	Normal/occlusion/emissive
	*/
	glm::vec3						emissive_factor;
	wptr<vlk_texture>				emissive_texture;
	wptr<vlk_texture>				normal_texture;
	wptr<vlk_texture>				occlusion_texture;

	/*
	pbrMetallicRoughness
	*/
	glm::vec4						base_color_factor;
	wptr<vlk_texture>				base_color_texture;
	float							metallic_factor;
	wptr<vlk_texture>				metallic_roughness_texture;
	float							roughness_factor;
};

/**
This maps the UBO layout as used by the shader.
*/
struct vlk_material_ubo
{
	glm::vec4		base_color_factor;
	glm::vec3		emissive_factor;
	float			metallic_factor;
	float			roughness_factor;
};

class vlk_material : public gpu_material {

public:

	vlk_material(vlk_device& device, const vlk_material_create_info& create_info);
	virtual ~vlk_material() override;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/** Binds the descriptor set for the material for rendering. */
	void bind
		(
		const vlk_frame&			frame,
		VkPipelineLayout			pipeline_layout
		) const;

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_buffers();
	void create_sets();
	void destroy_buffers();
	void destroy_sets();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					_device;

	/*
	Create/destroy
	*/
	std::vector<uptr<vlk_buffer>>	_buffers;
	std::vector<VkDescriptorSet>	_sets;

	/*
	Normal/occlusion/emissive
	*/
	glm::vec3					_emissive_factor;
	wptr<vlk_texture>			_emissive_texture;
	wptr<vlk_texture>			_normal_texture;
	wptr<vlk_texture>			_occlusion_texture;

	/*
	pbrMetallicRoughness
	*/
	glm::vec4					_base_color_factor;
	wptr<vlk_texture>			_base_color_texture;
	float						_metallic_factor;
	wptr<vlk_texture>			_metallic_roughness_texture;
	float						_roughness_factor;
};

}   /* namespace jetz */
