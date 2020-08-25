/*=============================================================================
app.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/app.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

app::app()
{
}

app::~app()
{
}

void app::run_frame()
{
	///* Get frame time delta */
	//j->frame_delta_time = g_platform->get_delta_time(g_platform);

	///* Begin frame */
	//gpu_frame_t* frame = gpu_window__begin_frame(&j->window.gpu_window, &j->camera, j->frame_delta_time);

	//player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	//render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

	///* End frame */
	//gpu_window__end_frame(&j->window.gpu_window, frame);
}

bool app::should_exit()
{
	return false;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
