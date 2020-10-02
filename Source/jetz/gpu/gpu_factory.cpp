/*=============================================================================
gpu_model.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_factory.h"
#include "jetz/main/log.h"
#include "jetz/main/utl.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

gpu_factory::gpu_factory()
{
}

gpu_factory::~gpu_factory()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

uptr<gpu_model> gpu_factory::load_gltf(const std::string& filename)
{
	auto gltf = uptr<tinygltf::Model>(new tinygltf::Model());
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;
	bool loadSuccess(false);

	/*
	Load and parse model file
	*/
	if (utl::ends_with(filename, ".glb"))
	{
		/* GLTF binary format */
		loadSuccess = loader.LoadBinaryFromFile(gltf.get(), &err, &warn, filename);
	}
	else
	{
		/* Regular ASCII GLTF format */
		loadSuccess = loader.LoadASCIIFromFile(gltf.get(), &err, &warn, filename);
	}

	if (!err.empty() || !loadSuccess)
	{
		LOG_ERROR_FMT("Failed to load GLTF model: {0}", err);
		
		// TODO : Have a hard-coded default model to use when there is an error (like a simple cube)
		return nullptr;
	}

	if (!warn.empty())
	{
		LOG_WARN(warn);
	}

	return create_model(std::move(gltf));
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
