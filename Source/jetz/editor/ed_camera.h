/*=============================================================================
ed_camera.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/camera.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

class ed_camera : public camera {

public:

	ed_camera();
	~ed_camera();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/
	void move(float moveDelta);
	void pan(float vertDelta, float horizDelta);
	void rot_x(float deltaX);
	void rot_y(float deltaY);

private:

	/*-----------------------------------------------------
	Private Variables
	-----------------------------------------------------*/
	float		_rotX;		/* degrees rotation on x axis */
	float		_rotY;		/* degrees rotation on y axis */
};

}   /* namespace jetz */
