/*=============================================================================
ed.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_frame.h"
#include "jetz/main/app.h"
#include "jetz/main/world.h"
#include "jetz/editor/ed.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ed::ed(app& app)
	: 
	_app(app)
{
}

ed::~ed()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

ed_camera& ed::get_camera()
{
	return _camera;
}

void ed::think(const gpu_frame& frame)
{
	update_camera(frame);
	//ed_file_picker.set_directory("worlds");
	//ed_file_picker.open();

	/*
	World file picker dialog
	*/
	if (_world_file_picker.think() == ed_dialog_result::OK)
	{
		//is_world_loading = true;
		//world_filename = ed_file_picker.get_selected_file();
		//LOG_DBG_FMT("Opened {0}", world_filename.c_str());
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void ed::update_camera(const gpu_frame& frame)
{
	auto& input = _app.get_world().get_ecs().input_singleton;

	if (input.key_down[GLFW_KEY_W])
	{
		_camera.move(frame.frame_time_delta * 2.0f);
	}
	else if (input.key_down[GLFW_KEY_S])
	{
		_camera.move(frame.frame_time_delta * -2.f);
	}

	if (!input.mouse_pos_changed)
	{
		return;
	}

	float moveSen = 0.05f;
	float rotSen = 0.30f;

	glm::vec2 prevMouse = input.mouse_pos_prev;
	float x = input.mouse_pos.x;
	float y = input.mouse_pos.y;
	bool lmb = input.mouse_down[GLFW_MOUSE_BUTTON_LEFT];
	bool rmb = input.mouse_down[GLFW_MOUSE_BUTTON_RIGHT];

	/* RIGHT + LEFT mouse buttons */
	if (rmb && lmb)
	{
		float vertDelta = ((float)y - prevMouse.y) * moveSen * -1.0f;
		float horizDelta = ((float)x - prevMouse.x) * moveSen;

		_camera.pan(vertDelta, horizDelta);
	}
	/* RIGHT mouse button */
	else if (rmb)
	{
		// Calc mouse change
		float rotDeltaX = ((float)y - prevMouse.y) * rotSen * -1.0f;
		float rotDeltaY = ((float)x - prevMouse.x) * rotSen * -1.0f;

		_camera.rot_x(rotDeltaX);
		_camera.rot_y(rotDeltaY);
	}
	/* LEFT mouse button */
	else if (lmb)
	{
		// Calc mouse change
		float rotDeltaY = ((float)x - prevMouse.x) * rotSen * -1.0f;
		float moveDelta = ((float)y - prevMouse.y) * moveSen * -1.0f;

		_camera.rot_y(rotDeltaY);
		_camera.move(moveDelta);
	}
}

}   /* namespace jetz */
