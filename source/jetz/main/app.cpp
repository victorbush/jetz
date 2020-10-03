/*=============================================================================
app.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/gpu/gpu_window.h"
#include "jetz/main/app.h"
#include "jetz/main/camera.h"
#include "jetz/main/log.h"
#include "jetz/main/window.h"
#include "thirdparty/glfw/glfw.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

app::app(window& main_window, gpu& gpu) :
	_gpu(gpu),
	_window(main_window),
	_frame_time(0),
	_frame_time_delta(0),
	_camera(),
	_should_exit(false),
	_state(app_state::STARTUP),
	_loader_system()
{
	_window.set_input_system(&_input_system);
	_window.set_window_close_callback(std::bind(&app::on_main_window_close, this));
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

	/* Run input system - needs to be done before start of frame */
	_input_system.run(&_world.get_ecs());

	/* Begin frame */
	auto gpu_window = _window.get_gpu_window().lock();
	gpu_frame& frame = gpu_window->begin_frame(_camera);

	imgui_begin_frame(_frame_time_delta, (float)_window.get_width(), (float)_window.get_height());

	


	/* 
	Determine what to do based on app state
	*/
	if (_state == app_state::STARTUP)
	{
		_state = app_state::EDITOR_LOADING;
	}
	else if (_state == app_state::EDITOR_LOADING)
	{
		// For now, just load a world
		world::load(_world, "worlds/world.lua");

		_state = app_state::EDITOR_RUNNING;
	}
	else if (_state == app_state::EDITOR_RUNNING)
	{
		/* Let loader system run */
		_loader_system.run(_world.get_ecs(), _gpu);

		/* Process editor UI and functionality */
		_ed.think();


		//////////

		/// TODO : Make this into a system
		auto& input = _world.get_ecs().input_singleton;
		if (input.key_down[GLFW_KEY_W])
		{
			_camera.move(_frame_time_delta * 2.0f);
		}
		else if (input.key_down[GLFW_KEY_S])
		{
			_camera.move(_frame_time_delta * -2.f);
		}


		//float moveSen = 0.05f;
		//float rotSen = 0.30f;

		//glm::vec2 prevMouse = manager.GetPrevMousePos();

		//bool lmb = manager.GetLeftMouseButtonState();
		//bool rmb = manager.GetRightMouseButtonState();

		///* RIGHT + LEFT mouse buttons */
		//if (rmb && lmb)
		//{
		//	float vertDelta = ((float)y - prevMouse.y) * moveSen * -1.0f;
		//	float horizDelta = ((float)x - prevMouse.x) * moveSen;

		//	_camera.Pan(vertDelta, horizDelta);
		//}
		///* RIGHT mouse button */
		//else if (rmb)
		//{
		//	// Calc mouse change
		//	float rotDeltaX = ((float)y - prevMouse.y) * rotSen * -1.0f;
		//	float rotDeltaY = ((float)x - prevMouse.x) * rotSen * -1.0f;

		//	_camera.RotX(rotDeltaX);
		//	_camera.RotY(rotDeltaY);
		//}
		///* LEFT mouse button */
		//else if (lmb)
		//{
		//	// Calc mouse change
		//	float rotDeltaY = ((float)x - prevMouse.x) * rotSen * -1.0f;
		//	float moveDelta = ((float)y - prevMouse.y) * moveSen * -1.0f;

		//	_camera.RotY(rotDeltaY);
		//	_camera.Move(moveDelta);
		//}


		//////////




		_render_system.run(_world.get_ecs(), _gpu, frame);
	}

	//player_system__run(&j->world.ecs, &j->camera, j->frame_delta_time);
	//render_system__run(&j->world.ecs, &j->window.gpu_window, frame);

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

void app::imgui_end_frame(sptr<gpu_window> window, gpu_frame& frame)
{
	/* End imgui frame */
	ImGui::EndFrame();
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	window->render_imgui(frame, draw_data);
}

void app::on_main_window_close()
{
	_should_exit = true;
}

}   /* namespace jetz */
