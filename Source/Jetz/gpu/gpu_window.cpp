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

gpu_frame& gpu_window::begin_frame(camera* cam)
{
	/* Get next frame */
	gpu_frame& frame = _frames[_frame_idx];
	frame.frame_idx = _frame_idx;

	/* Pass to renderer */
	this->do_begin_frame(frame, cam);

	/* Update frame index */
	_frame_idx = (_frame_idx + 1) % gpu::num_frame_buf;

	return frame;
}

void gpu_window::end_frame(gpu_frame& frame)
{
	this->do_end_frame(frame);
}

void gpu_window::render_imgui(gpu_frame* frame, ImDrawData* draw_data)
{
}

void gpu_window::resize(uint32_t width, uint32_t height)
{
	_height = height;
	_width = width;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
