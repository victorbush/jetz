/*=============================================================================
vlk_texture.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory>

#include "jetz/gpu/vlk/vlk_model.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_model::vlk_model
	(
	vlk_device&					dev,
	uptr<tinygltf::Model>		gltf
	)
	: 
	_device(dev),
	_gltf(std::move(gltf))
{
	create_buffers();
	create_vertex_input_bindings();
	create_textures();
	create_materials();
	create_pipelines();
}

vlk_model::~vlk_model()
{
	destroy_pipelines();
	destroy_materials();
	destroy_textures();
	destroy_vertex_input_bindings();
	destroy_buffers();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void vlk_model::create_buffers()
{
	auto numBuffers = _gltf->buffers.size();
	_buffers.resize(numBuffers);

	// TODO : Sometimes a buffer contains more than indices and vertex data (ie,
	// when textures are embedded into buffers for binary gltf

	/* Create and populate each buffer */
	for (size_t i = 0; i < numBuffers; ++i)
	{
		const auto& b = _gltf->buffers[i];
		size_t buf_size = b.data.size();

		/* Create buffer */
		auto vlk_buf = new vlk_buffer(
			_device,
			buf_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY);
			
		_buffers[i] = uptr<vlk_buffer>(vlk_buf);

		/* Transfer data to GPU */
		_buffers[i]->update((void*)b.data.data(), 0, buf_size);
	}
}

void vlk_model::destroy_buffers()
{
	for (size_t i = 0; i < _buffers.size(); ++i)
	{
		_buffers[i].reset();
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
