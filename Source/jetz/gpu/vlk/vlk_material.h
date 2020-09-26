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
#include "jetz/gpu/vlk/vlk_device.h"
#include "jetz/gpu/vlk/vlk_texture.h"
#include "jetz/main/common.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

class vlk_material : public gpu_material {

public:

	vlk_material(const vlk_device& device);
	virtual ~vlk_material() override;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

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

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/** Binds the descriptor set for the material for rendering. */
	void bind_descriptor_set();

	/**
	Updates the descriptor set using the current material properties.
	This will update the data in the material UBO as well.
	*/
	void update_descriptor_set();

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	const vlk_device&			_device;
};

}   /* namespace jetz */
