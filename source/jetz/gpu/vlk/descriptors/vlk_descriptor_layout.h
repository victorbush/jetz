/*=============================================================================
vlk_descriptor_layout.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class vlk_device;

/*=============================================================================
CLASS
=============================================================================*/

class vlk_descriptor_layout {

public:

	vlk_descriptor_layout
		(
		vlk_device& dev
		);

	virtual ~vlk_descriptor_layout() = 0;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	VkDescriptorSetLayout	get_handle() const;
	vlk_device&				get_device() const;
	VkDescriptorPool		get_pool_handle() const;

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

	/** Creates a descriptor pool for this layout. Descriptor sets are allocated from the pool. */
	virtual void create_descriptor_pool() = 0;

	/** Creates the descriptor set layout. */
	virtual void create_layout() = 0;

	/** Destroys the descriptor pool. */
	void destroy_descriptor_pool();

	/** Destroys the descriptor set layout. */
	void destroy_layout();

	/*-----------------------------------------------------
	Protected variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					dev;

	/*
	Create/destroy
	*/
	VkDescriptorSetLayout		handle;
	VkDescriptorPool			pool_handle;
};

}   /* namespace jetz */
