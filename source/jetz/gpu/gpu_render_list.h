/*=============================================================================
gpu_render_list.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <cstdint>
#include <vector>

#include "jetz/main/common.h"
#include "jetz/ecs/components/ecs_transform_component.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class camera;
class gpu_frame;
class gpu_model;

/*=============================================================================
TYPES
=============================================================================*/

class gpu_render_list {

public:

	std::vector<wptr<gpu_model>> models;
	std::vector<ecs_transform_component> model_transforms;

	std::vector<wptr<

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
