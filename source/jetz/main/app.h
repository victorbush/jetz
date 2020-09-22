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
#include "jetz/ecs/systems/ecs_loader_system.h"
#include "jetz/gpu/vlk/vlk_frame.h"
#include "jetz/main/camera.h"
#include "jetz/main/world.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class gpu;
class gpu_window;
class window;
class world;

/*=============================================================================
TYPES
=============================================================================*/

enum class app_state {
	STARTUP,
	//MENU_LOADING,
	//MENU_RUNNING,
	EDITOR_LOADING,
	EDITOR_RUNNING,
	//GAME_LOADING,
	//GAME_PLAYING,
	//GAME_PAUSED,
	SHUTDOWN
};

class app {

public:

	app(window& main_window, gpu& gpu);
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
	gpu&					_gpu;
	window&					_window;

	/*
	Systems
	*/
	ecs_input_system		_input_system;
	ecs_loader_system		_loader_system;

	/*
	Other
	*/
	camera					_camera;
	ecs						_ecs;				/* Entity component system */
	ed						_ed;				/* Editor */
	float					_frame_time;		/* Current frame time (in seconds) */
	float					_frame_time_delta;	/* Delta time since the last frame (in seconds) */

	bool					_should_exit;		/* Should the application exit? */
	app_state				_state;
	world					_world;
};

}   /* namespace jetz */
