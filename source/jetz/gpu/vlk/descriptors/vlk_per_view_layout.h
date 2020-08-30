/*=============================================================================
vlk_per_view_layout.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "jetz/gpu/vlk/descriptors/vlk_descriptor_layout.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
class vlk_device;

/*=============================================================================
CLASS
=============================================================================*/

struct vlk_per_view_ubo {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camera_pos;
};

class vlk_per_view_layout : public vlk_descriptor_layout {

public:

	vlk_per_view_layout
		(
		vlk_device& dev
		);

	~vlk_per_view_layout();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

	/** Creates a descriptor pool for this layout. Descriptor sets are allocated from the pool. */
	virtual void create_descriptor_pool() override;

	/** Creates the descriptor set layout. */
	virtual void create_layout() override;

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
};

}   /* namespace jetz */
