/*=============================================================================
gpu_window.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu.h"
#include "jetz/gpu/gpu_frame.h"
#include "jetz/gpu/gpu_window.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

gpu_window::gpu_window(uint32_t width, uint32_t height) :
	_height(height),
	_width(width),
	_frame_idx(0)
{
	_frames.resize(gpu::num_frame_buf);
}

gpu_window::~gpu_window()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

gpu_frame& gpu_window::begin_frame(camera* cam, float delta_time)
{
	/* Get next frame */
	gpu_frame& frame = _frames[_frame_idx];
	frame.frame_idx = frame_idx;

}

void gpu_window::end_frame(gpu_frame* frame)
{
}

void gpu_window::render_imgui(gpu_frame* frame, ImDrawData* draw_data)
{
}

void gpu_window::resize(uint32_t width, uint32_t height)
{
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
