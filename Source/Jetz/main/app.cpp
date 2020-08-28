/*=============================================================================
app.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_window.h"
#include "jetz/main/app.h"
#include "jetz/main/window.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

app::app(window& main_window) :
	_window(main_window),
	_frame_time(0),
	_frame_time_delta(0)
{
}

app::~app()
{
}

void app::run_frame()
{
	/* Get frame time delta */
	float last_time = _frame_time;
	_frame_time = (float)glfwGetTime();
	_frame_time_delta = _frame_time - last_time;

	/* Begin frame */
	//_window.get_gpu_window()
	//gpu_frame_t* frame = gpu_window__begin_frame(&j->window.gpu_window, &j->camera, j->frame_delta_time);

	//player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	//render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

	///* End frame */
	//gpu_window__end_frame(&j->window.gpu_window, frame);
}

bool app::should_exit()
{
	return _should_exit;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
