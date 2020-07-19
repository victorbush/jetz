/*=============================================================================
vlk.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "jetz/gpu/gpu.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk : gpu {

public:

	vlk();
	~vlk();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	jetz::gpu::wait_idle
	*/
	virtual void wait_idle() const;

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

	/** Enable Vulkan validation layers? */
	bool enable_validation;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::vector<const char*> required_device_ext;		/* required device extensions */
	std::vector<const char*> required_instance_ext;		/* required instance extensions */
	std::vector<const char*> required_instance_layers;	/* required instance layers */

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void create_requirement_lists();
	//void create_instance();
	//void create_dbg_callbacks();
	//void create_device();
};

}   /* namespace jetz */
