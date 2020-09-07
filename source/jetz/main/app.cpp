/*=============================================================================
app.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_window.h"
#include "jetz/main/app.h"
#include "jetz/main/camera.h"
#include "jetz/main/window.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

app::app(window& main_window) :
	_window(main_window),
	_frame_time(0),
	_frame_time_delta(0),
	_camera(),
	_should_exit(false)
{
}

app::~app()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void app::run_frame()
{
	/* Get frame time delta */
	float last_time = _frame_time;
	_frame_time = (float)glfwGetTime();
	_frame_time_delta = _frame_time - last_time;

	/* Begin frame */
	gpu_window* gpu_window = _window.get_gpu_window();
	gpu_frame& frame = gpu_window->begin_frame(_camera);

	//player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	//render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

	/* End frame */
	gpu_window->end_frame(frame);
}

bool app::should_exit()
{
	return _should_exit;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
