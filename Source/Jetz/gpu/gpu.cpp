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
	// Destructors of elements are called by clear()
	materials.clear();
}

void gpu::unload_models()
{
	// Destructors of elements are called by clear()
	models.clear();
}

void gpu::unload_textures()
{
	// Destructors of elements are called by clear()
	textures.clear();
}

}   /* namespace jetz */
