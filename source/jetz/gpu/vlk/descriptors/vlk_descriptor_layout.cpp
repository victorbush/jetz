/*=============================================================================
vlk_descriptor_layout.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk.h"
#include "jetz/gpu/vlk/descriptors/vlk_descriptor_layout.h"
#include "jetz/main/common.h"
#include "jetz/main/log.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_descriptor_layout::vlk_descriptor_layout(vlk_device& dev)
	: dev(dev),
	handle(VK_NULL_HANDLE),
	pool_handle(VK_NULL_HANDLE)
{
}

vlk_descriptor_layout::~vlk_descriptor_layout() 
{
	destroy_layout();
	destroy_descriptor_pool();
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

VkDescriptorSetLayout vlk_descriptor_layout::get_handle() const
{
	return handle;
}

vlk_device& vlk_descriptor_layout::get_device() const
{
	return dev;
}

VkDescriptorPool vlk_descriptor_layout::get_pool_handle() const
{
	return pool_handle;
}

/*=============================================================================
PROTECTED METHODS
=============================================================================*/

void vlk_descriptor_layout::destroy_descriptor_pool()
{
	vkDestroyDescriptorPool(dev.get_handle(), pool_handle, NULL);
}

void vlk_descriptor_layout::destroy_layout()
{
	vkDestroyDescriptorSetLayout(dev.get_handle(), handle, NULL);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
