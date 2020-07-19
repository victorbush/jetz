/*=============================================================================
vlk_util.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory.h>

#include "jetz/gpu/vlk/vlk_util.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC STATIC METHODS
=============================================================================*/

bool vlk_util::are_instance_extensions_available
	(
	const std::vector<const char*>&		extensions
	)
{
	LOG_FATAL("NOT IMPLEMENTED");
	return false;
}

bool vlk_util::are_instance_layers_available
	(
	const std::vector<const char*>&		layers
	)
{
	bool desired_layers_available = true;

	/* get number of available instance layers */
	uint32_t count;
	vkEnumerateInstanceLayerProperties(&count, NULL);

	/* get list of available instance layers*/
	VkLayerProperties* available_layers;
	vkEnumerateInstanceLayerProperties(&count, NULL);
	available_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * count);
	vkEnumerateInstanceLayerProperties(&count, available_layers);

	/* check if specified layers are available */
	for (const auto* layer_name : layers)
	{
		bool layer_found = false;

		/* look for this layer in the available layers */
		for (uint32_t j = 0; j < count; ++j)
		{
			VkLayerProperties layer = available_layers[j];

			if (!strncmp(layer_name, layer.layerName, sizeof(layer.layerName)))
			{
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
		{
			desired_layers_available = false;
			break;
		}
	}

	free(available_layers);

	return (desired_layers_available);
}

}   /* namespace jetz */
