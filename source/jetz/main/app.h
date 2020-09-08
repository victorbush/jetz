/*=============================================================================
app.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <vulkan/vulkan.h>

#include "jetz/editor/ed.h"
#include "jetz/ecs/ecs.h"
#include "jetz/ecs/systems/ecs_input_system.h"
#include "jetz/main/camera.h"
#include "jetz/gpu/vlk/vlk_frame.h"

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
	//float			simulation_speed;

	//bool			is_world_loading;
	//std::string		world_filename;




private:

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void imgui_begin_frame(float delta_time, float width, float height);
	void imgui_end_frame(gpu_window* window, gpu_frame& frame);
	void on_main_window_close();

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*
	Dependencies
	*/
	window&					_window;

	/*
	Systems
	*/
	ecs_input_system		_input_system;

	/*
	Other
	*/
	camera					_camera;
	ecs						_ecs;				/* Entity component system */
	ed						_ed;				/* Editor */
	float					_frame_time;		/* Current frame time (in seconds) */
	float					_frame_time_delta;	/* Delta time since the last frame (in seconds) */

	bool					_should_exit;		/* Should the application exit? */
};

}   /* namespace jetz */
