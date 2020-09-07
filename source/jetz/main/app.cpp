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
	ed_file_picker.open();
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

	imgui_begin_frame(_frame_time_delta, _window.get_width(), _window.get_height());

	//player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	//render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

	ed_file_picker.think();

	imgui_end_frame(gpu_window, frame);

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

void app::imgui_begin_frame(float delta_time, float width, float height)
{
	ImGuiIO& io = ImGui::GetIO(); 

	// Setup low-level inputs, e.g. on Win32: calling GetKeyboardState(), or write to those fields from your Windows message handlers, etc.
	// (In the examples/ app this is usually done within the ImGui_ImplXXX_NewFrame() function from one of the demo Platform bindings)
	io.DeltaTime = delta_time;              // set the time elapsed since the previous frame (in seconds)
	io.DisplaySize.x = width;				// set the current display width
	io.DisplaySize.y = height;             // set the current display height here

	// Call NewFrame(), after this point you can use ImGui::* functions anytime
	// (So you want to try calling NewFrame() as early as you can in your mainloop to be able to use imgui everywhere)
	ImGui::NewFrame();
}

void app::imgui_end_frame(gpu_window* window, gpu_frame& frame)
{
	/* End imgui frame */
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	window->render_imgui(frame, draw_data);
}

}   /* namespace jetz */
