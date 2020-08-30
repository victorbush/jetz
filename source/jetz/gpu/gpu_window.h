/*=============================================================================
gpu_window.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <cstdint>
#include <vector>

#include "thirdparty/imgui/imgui.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class camera;
class gpu_frame;

/*=============================================================================
TYPES
=============================================================================*/

class gpu_window {

public:

	gpu_window(uint32_t width, uint32_t height);
	virtual ~gpu_window();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	gpu_frame& begin_frame(camera& cam);
	void end_frame(const gpu_frame& frame);
	void render_imgui(const gpu_frame& frame, ImDrawData* draw_data);
	void resize(uint32_t width, uint32_t height);

protected:

	/*-----------------------------------------------------
	Protected methods
	-----------------------------------------------------*/

	virtual gpu_frame& do_begin_frame(camera& cam) = 0;
	virtual void do_end_frame(const gpu_frame& frame) = 0;
	virtual void do_render_imgui(const gpu_frame& frame, ImDrawData* draw_data) = 0;
	virtual void do_resize() = 0;

	/*-----------------------------------------------------
	Protected variables
	-----------------------------------------------------*/

	uint8_t						_frame_idx;			/* Index of the current frame. */
	uint32_t					_height;
	uint32_t					_width;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
};

}   /* namespace jetz */
