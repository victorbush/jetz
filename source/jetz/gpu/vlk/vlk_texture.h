/*=============================================================================
vlk_texture.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/gpu/gpu_texture.h"
#include "jetz/gpu/vlk/vlk_device.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

struct vlk_texture_create_info
{
	void*						data;		/* pointer to texture data */
	uint32_t					height;		/* height of texture in pixels */
	size_t						size;		/* size of texture data in bytes */
	uint32_t					width;		/* width of texture in pixels */
};

class vlk_texture : public gpu_texture {

public:

	vlk_texture
		(
		vlk_device&						device,
		const vlk_texture_create_info&	create_info
		);

	virtual ~vlk_texture() override;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	VkImage get_image() const;
	VkDescriptorImageInfo* get_image_info();

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	/** Creates the texture image. */
	void create_image(const vlk_texture_create_info& create_info);

	/** Creates the texture image view. */
	void create_image_view();

	/** Initializes the image info data for the texture. */
	void init_image_info();

	/** Destroys the texture image. */
	void destroy_image();

	/** Destroys the texture image view. */
	void destroy_image_view();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	vlk_device&					dev;
	VkFormat					format;

	/*
	Create/destroy
	*/
	VkImage						image;
	VmaAllocation				image_allocation;
	VkImageView					image_view;

	/*
	Other
	*/
	VkDescriptorImageInfo		image_info;
};

}   /* namespace jetz */
