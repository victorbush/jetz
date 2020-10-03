/*=============================================================================
ecs_input_system.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs;

class ecs_input_system {

public:

	ecs_input_system();
	~ecs_input_system();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	void run(ecs* ecs);

	void on_char(unsigned int c);
	void on_key(int key, int scancode, int action, int mods);
	void on_mouse_button(int button, int action, int mods);
	void on_mouse_move(double xpos, double ypos);
	void on_mouse_scroll(double xoffset, double yoffset);

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	ecs*		_current_ecs;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

};

}   /* namespace jetz */
