/*=============================================================================
vlk_util.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vector>
#include <vulkan/vulkan.h>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
CLASS
=============================================================================*/

class vlk_util {

public:

	/*-----------------------------------------------------
	Public static methods
	-----------------------------------------------------*/

	/**
	Checks if a specified list of instance extensions are available.
	*/
	static bool are_instance_extensions_available(const std::vector<const char*>& extensions);

	/**
	Checks if a specified list of instance layers are available.
	*/
	static bool are_instance_layers_available(const std::vector<const char*>& layers);
};

}   /* namespace jetz */
