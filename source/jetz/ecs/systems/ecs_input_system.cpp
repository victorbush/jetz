/*=============================================================================
ecs_input_system.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/components/ecs_input_singleton.h"
#include "jetz/ecs/systems/ecs_input_system.h"
#include "jetz/main/log.h"
#include "thirdparty/glfw/glfw.h"
#include "thirdparty/imgui/imgui.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ecs_input_system::ecs_input_system()
	: 
	_current_ecs(nullptr)
{
}

ecs_input_system::~ecs_input_system()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void ecs_input_system::run(ecs* ecs)
{
	/* Set current ecs. GLFW callbacks will use it. */
	_current_ecs = ecs;

	ecs->input_singleton.mouse_pos_changed = false;

	glfwPollEvents();
}

void ecs_input_system::on_char(unsigned int c)
{
	/* imgui */
	ImGuiIO& imgui = ImGui::GetIO();

	if (c > 0 && c < 0x10000)
	{
		imgui.AddInputCharacter(c);
	}
}

void ecs_input_system::on_key(int key, int scancode, int action, int mods)
{
	if (!_current_ecs)
	{
		LOG_ERROR("No ECS found in input system.");
		return;
	}

	ImGuiIO& imgui = ImGui::GetIO();
	auto& input = _current_ecs->input_singleton;

	if (action == GLFW_PRESS)
	{
		input.key_down[key] = true;
		imgui.KeysDown[key] = true;
	}

	if (action == GLFW_RELEASE)
	{
		input.key_down[key] = false;
		imgui.KeysDown[key] = false;
	}

	/* According to ImGui example, modifiers are not reliable across systems */
	imgui.KeyCtrl = imgui.KeysDown[GLFW_KEY_LEFT_CONTROL] || imgui.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	imgui.KeyShift = imgui.KeysDown[GLFW_KEY_LEFT_SHIFT] || imgui.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	imgui.KeyAlt = imgui.KeysDown[GLFW_KEY_LEFT_ALT] || imgui.KeysDown[GLFW_KEY_RIGHT_ALT];
	imgui.KeySuper = imgui.KeysDown[GLFW_KEY_LEFT_SUPER] || imgui.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ecs_input_system::on_mouse_button(int button, int action, int mods)
{
	if (!_current_ecs)
	{
		LOG_ERROR("No ECS found in input system.");
		return;
	}

	auto& input = _current_ecs->input_singleton;
	input.mouse_down[button] = action == GLFW_PRESS || action == GLFW_REPEAT;

	/* imgui */
	ImGuiIO& imgui = ImGui::GetIO();
	imgui.MouseDown[0] = input.mouse_down[GLFW_MOUSE_BUTTON_LEFT];
	imgui.MouseDown[1] = input.mouse_down[GLFW_MOUSE_BUTTON_RIGHT];
}

void ecs_input_system::on_mouse_move(double xpos, double ypos)
{
	if (!_current_ecs)
	{
		LOG_ERROR("No ECS found in input system.");
		return;
	}

	auto& input = _current_ecs->input_singleton;
	input.mouse_pos_prev = input.mouse_pos;
	input.mouse_pos.x = (float)xpos;
	input.mouse_pos.y = (float)ypos;
	input.mouse_pos_changed = true;

	/* imgui */
	ImGuiIO& imgui = ImGui::GetIO();
	imgui.MousePos.x = (float)xpos;
	imgui.MousePos.y = (float)ypos;
}

void ecs_input_system::on_mouse_scroll(double xoffset, double yoffset)
{
	/* imgui */
	ImGuiIO& imgui = ImGui::GetIO();
	imgui.MouseWheelH += (float)xoffset;
	imgui.MouseWheel += (float)yoffset;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
