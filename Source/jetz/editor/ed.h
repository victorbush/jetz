/*=============================================================================
ed.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/editor/ed_file_picker_dialog.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class app;
class gpu_frame;

class ed {

public:

	ed(app& app);
	~ed();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	void think(const gpu_frame& frame);

	/*-----------------------------------------------------
	Public variables
	-----------------------------------------------------*/

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/
	app&					_app;
	bool					_camera_is_moving;		/** The user is moving the camera (don't try to select an entity) */
	ed_file_picker_dialog	_world_file_picker;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/
	void update_camera(const gpu_frame& frame);
};

}   /* namespace jetz */
