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
int gpu::num_frame_buf = 2;

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

gpu::gpu()
{
}

gpu::~gpu()
{
}

gpu_material* gpu::load_material(const std::string& filename)
{
	return nullptr;
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
	for (auto mat : materials)
	{
		delete mat.second;
	}

	materials.clear();
}

void gpu::unload_models()
{
	for (auto model : models)
	{
		delete model.second;
	}

	models.clear();
}

void gpu::unload_textures()
{
	for (auto tex : textures)
	{
		delete tex.second;
	}

	textures.clear();
}

}   /* namespace jetz */
