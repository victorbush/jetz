/*=============================================================================
vlk_model.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/main/common.h"
#include "jetz/gpu/gpu_model.h"
#include "jetz/gpu/vlk/vlk_buffer.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "thirdparty/tinygltf/tiny_gltf.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

class vlk_model : public gpu_model {

public:

	vlk_model(vlk_device& dev, uptr<tinygltf::Model> gltf);
	virtual ~vlk_model() override;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_buffers();
	void destroy_buffers();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					_device;
	uptr<tinygltf::Model>		_gltf;

	/*
	Create/destroy
	*/
	std::vector<uptr<vlk_buffer>>	_buffers;

	/*
	Other
	*/
};

}   /* namespace jetz */
