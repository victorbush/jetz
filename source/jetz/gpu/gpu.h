/*=============================================================================
gpu.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <unordered_map>
#include <string>

#include "jetz/gpu/gpu_factory.h"
#include "jetz/gpu/gpu_material.h"
#include "jetz/gpu/gpu_model.h"
#include "jetz/gpu/gpu_texture.h"
#include "jetz/main/common.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class gpu {

public:

	gpu(gpu_factory& gpu_factory);
	virtual ~gpu() = 0;

	/*-----------------------------------------------------
	Public static variables
	-----------------------------------------------------*/

	/* The maximum number of materials allowed. */
	static int max_num_materials;

	/**
	The number of frame buffers to use for rendering.
	Double buffered == 2
	Triple buffered == 3
	*/
	static uint8_t num_frame_buf;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	/**
	Returns the specified material. If the material has not been loaded, null will be returned.

	@param filename The material to get.
	@returns The loaded material if found, NULL otherwise.
	*/
	wptr<gpu_material> get_material(const std::string& filename);

	/**
	Returns the specified model. If the model has not been loaded, null will be returned.

	@param filename The model to get.
	@returns The loaded model if found, NULL otherwise.
	*/
	wptr<gpu_model> get_model(const std::string& filename);

	/**
	Returns the specified texture. If the texture has not been loaded, null will be returned.

	@param filename The texture to get.
	@returns The loaded texture if found, NULL otherwise.
	*/
	wptr<gpu_texture> get_texture(const std::string& filename);

	/**
	Returns the specified material, loading it if needed.

	@param filename The material file to load.
	@returns The loaded material if found, NULL otherwise.
	*/
	wptr<gpu_material> load_material(const std::string& filename);

	/**
	Returns the specified model, loading it if needed.

	@param filename The model file to load.
	@returns The loaded model if found, NULL otherwise.
	*/
	wptr<gpu_model> load_model(const std::string& filename);

	/**
	Returns the specified texture, loading it if needed.

	@param filename The texture file to load.
	@returns The loaded texture if found, NULL otherwise.
	*/
	wptr<gpu_texture> load_texture(const std::string& filename);

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

	gpu_factory& _factory;

	/** Materials cache. */
	std::unordered_map<std::string, sptr<gpu_material>> _materials;

	/** Models cache. */
	std::unordered_map<std::string, sptr<gpu_model>> _models;

	/** Textures cache. */
	std::unordered_map<std::string, sptr<gpu_texture>> _textures;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/

	void unload_materials();
	void unload_models();
	void unload_textures();
};

}   /* namespace jetz */
