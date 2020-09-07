/*=============================================================================
app.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/ecs/ecs.h"
#include "jetz/main/camera.h"
#include "jetz/gpu/vlk/vlk_frame.h"


#include "jetz/editor/ed_file_picker_dialog.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class gpu_window;
class window;

/*=============================================================================
TYPES
=============================================================================*/

class app {

public:

	app(window& main_window);
	~app();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	void run_frame();
	bool should_exit();



	// TODO : MOVE
	float			simulation_speed;

	bool			is_world_loading;
	std::string		world_filename;


	ed_file_picker_dialog	ed_file_picker;


private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void imgui_begin_frame(float delta_time, float width, float height);
	void imgui_end_frame(gpu_window* window, gpu_frame& frame);

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	window&					_window;

	/*
	Other
	*/
	camera					_camera;
	ecs						_ecs;
	float					_frame_time;		/* Current frame time (in seconds) */
	float					_frame_time_delta;	/* Delta time since the last frame (in seconds) */

	bool					_should_exit;		/* Should the application exit? */
};

}   /* namespace jetz */
