/*=============================================================================
gpu.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

int gpu::max_num_materials = 100;

/* Default to double buffering. */
uint8_t gpu::num_frame_buf = 2;

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

gpu::gpu()
{
}

gpu::~gpu()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

wptr<gpu_material> gpu::get_material(const std::string& filename)
{
	/* Check if already loaded */
	if (_materials.find(filename) != _materials.end())
	{
		return _materials[filename];
	}

	return wptr<gpu_material>();
}

wptr<gpu_model> gpu::get_model(const std::string& filename)
{
	/* Check if already loaded */
	if (_models.find(filename) != _models.end())
	{
		return _models[filename];
	}

	return wptr<gpu_model>();
}

wptr<gpu_texture> gpu::get_texture(const std::string& filename)
{
	/* Check if already loaded */
	if (_textures.find(filename) != _textures.end())
	{
		return _textures[filename];
	}

	return wptr<gpu_texture>();
}

wptr<gpu_material> gpu::load_material(const std::string& filename)
{
	LOG_FATAL("jetz::gpu::load_material");
	return wptr<gpu_material>();
}

wptr<gpu_model> gpu::load_model(const std::string& filename)
{
	/* Check if already loaded */
	auto model = get_model(filename);
	if (!model.expired())
	{
		return model;
	}

	/* Not loaded, load it now */
	auto new_model_uptr = get_factory()->load_gltf(filename);

	/* Convert to shared pointer and store in the model cache */
	auto new_model_sptr = _models[filename] = sptr<gpu_model>(std::move(new_model_uptr));

	return wptr<gpu_model>(new_model_sptr);
}

wptr<gpu_texture> jetz::gpu::load_texture(const std::string& filename)
{
	LOG_FATAL("jetz::gpu::load_texture");
	return wptr<gpu_texture>();
}

void gpu::wait_idle() const
{
	/* This gets overriden by dervied classes. Default behavior is to do nothing. */
}

/*=============================================================================
PROTECTED METHODS
=============================================================================*/

void gpu::unload_cache()
{
	wait_idle();

	unload_materials();
	unload_models();
	unload_textures();
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void gpu::unload_materials()
{
	for (auto mat : _materials)
	{
		mat.second.reset();
	}

	_materials.clear();
}

void gpu::unload_models()
{
	for (auto model : _models)
	{
		model.second.reset();
	}

	_models.clear();
}

void gpu::unload_textures()
{
	for (auto tex : _textures)
	{
		tex.second.reset();
	}

	_textures.clear();
}

}   /* namespace jetz */
