/*=============================================================================
gpu.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <unordered_map>
#include <string>

#include "jetz/gpu/gpu_material.h"
#include "jetz/gpu/gpu_model.h"
#include "jetz/gpu/gpu_texture.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class gpu {

public:

	gpu();
	virtual ~gpu() = 0;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Returns the specified material, loading it if needed.

	@param filename The material file to load.
	@returns [BORROW] The loaded material if found, NULL otherwise.
	*/
	gpu_material* load_material(const std::string& filename);

	/**
	Returns the specified model, loading it if needed.

	@param filename The model file to load.
	@returns [BORROW] The loaded model if found, NULL otherwise.
	*/
	gpu_model* load_model(const std::string& filename);

	/**
	Returns the specified texture, loading it if needed.

	@param filename The texture file to load.
	@returns [BORROW] The loaded texture if found, NULL otherwise.
	*/
	gpu_texture* load_texture(const std::string& filename);

	/**
	Waits until the GPU has finished executing the current command buffer.
	*/
	virtual void wait_idle() const;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

	/**
	Unloads any cached GPU resources (models, textures, etc).
	*/
	void unload_cache();

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/** Materials cache. */
	std::unordered_map<std::string, gpu_material*> materials;

	/** Models cache. */
	std::unordered_map<std::string, gpu_model*> models;

	/** Textures cache. */
	std::unordered_map<std::string, gpu_texture*> textures;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/

	void unload_materials();
	void unload_models();
	void unload_textures();
};

}   /* namespace jetz */
