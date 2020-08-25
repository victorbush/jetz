/*=============================================================================
vlk_material_layout.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

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

class vlk_material_layout : public vlk_descriptor_layout {

public:

	vlk_material_layout
		(
		vlk_device& dev
		);

	~vlk_material_layout();

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
