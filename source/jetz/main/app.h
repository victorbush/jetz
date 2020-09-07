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

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

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

private:

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
