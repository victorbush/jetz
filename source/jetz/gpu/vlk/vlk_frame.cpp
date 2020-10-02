/*=============================================================================
vlk_frame.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/vlk/vlk_frame.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

vlk_frame::vlk_frame(uint8_t frame_idx)
	: 
	_gpu_frame(frame_idx),
	cmd_buf(VK_NULL_HANDLE),
	picker_cmd_buf(VK_NULL_HANDLE),
	image_idx(0),
	delta_time(0.0)
{
}

vlk_frame::~vlk_frame()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

gpu_frame& vlk_frame::get_gpu_frame()
{
	return _gpu_frame;
}

uint8_t vlk_frame::get_frame_idx() const
{
	return _gpu_frame.get_frame_idx();
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
