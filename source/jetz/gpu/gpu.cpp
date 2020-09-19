/*=============================================================================
gpu.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

int gpu::max_num_materials = 100;

/* Default to double buffering. */
uint8_t gpu::num_frame_buf = 2;

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

gpu::gpu()
{
}

gpu::~gpu()
{
	unload_cache();
}

gpu_material* gpu::get_material(const std::string& filename)
{
	/* Check if material already loaded */
	if (_materials.find(filename) != _materials.end())
	{
		return _materials[filename];
	}

	return nullptr;
}

gpu_model* gpu::get_model(const std::string& filename)
{
	return nullptr;
}

gpu_model* gpu::get_texture(const std::string& filename)
{
	return nullptr;
}

gpu_material* gpu::load_material(const std::string& filename)
{
	/* Check if material already loaded */
	auto mat = get_material(filename);
	if (mat)
	{
		return mat;
	}

	/* Not loaded, load it now */
	mat = 
}

gpu_model* gpu::load_model(const std::string& filename)
{
	return nullptr;
}

gpu_texture* jetz::gpu::load_texture(const std::string& filename)
{
	return nullptr;
}

void gpu::wait_idle() const
{
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
		delete mat.second;
	}

	_materials.clear();
}

void gpu::unload_models()
{
	for (auto model : _models)
	{
		delete model.second;
	}

	_models.clear();
}

void gpu::unload_textures()
{
	for (auto tex : _textures)
	{
		delete tex.second;
	}

	_textures.clear();
}

}   /* namespace jetz */
