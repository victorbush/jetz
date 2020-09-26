/*=============================================================================
gpu_model.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_factory.h"
#include "jetz/gpu/vlk/vlk_model.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

uptr<gpu_model> vlk_factory::create_model(uptr<tinygltf::Model> gltf)
{
	auto model_ptr = new vlk_model(std::move(gltf));
	auto model = uptr<vlk_model>(model_ptr);

	return std::move(model);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */