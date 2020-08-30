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
}

gpu_window::~gpu_window()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

gpu_frame& gpu_window::begin_frame(camera& cam)
{
	/* Update frame index */
	_frame_idx = (_frame_idx + 1) % gpu::num_frame_buf;

	/* Pass to renderer */
	return this->do_begin_frame(cam);
}

void gpu_window::end_frame(const gpu_frame& frame)
{
	this->do_end_frame(frame);
}

void gpu_window::render_imgui(const gpu_frame& frame, ImDrawData* draw_data)
{
	this->do_render_imgui(frame, draw_data);
}

void gpu_window::resize(uint32_t width, uint32_t height)
{
	_height = height;
	_width = width;

	this->do_resize();
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
