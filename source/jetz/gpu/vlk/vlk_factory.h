/*=============================================================================
vlk_factory.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>

#include "jetz/main/common.h"
#include "jetz/gpu/gpu_factory.h"
#include "jetz/gpu/gpu_model.h"
#include "jetz/gpu/vlk/vlk_device.h"
#include "thirdparty/tinygltf/tiny_gltf.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_factory : public gpu_factory {

public:

	vlk_factory
		(
		vlk_device&					device
		);
	virtual ~vlk_factory();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	uptr<gpu_model> create_model(uptr<tinygltf::Model> gltf) override;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	vlk_device&					_device;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/
};

}   /* namespace jetz */
