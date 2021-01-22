/*=============================================================================
ed_camera.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/editor/ed_camera.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ed_camera::ed_camera()
{
	_rotX = 0.0f;
	_rotY = 180.0f;
}

ed_camera::~ed_camera()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void ed_camera::move(float moveDelta)
{
	// Movement only occurs on the X / Z axes. Kill movement on Y axis.
	glm::vec3 deltaVector = { _dir[0], 0, _dir[2] };
	deltaVector *= moveDelta;
	_pos += deltaVector;
}

void ed_camera::pan(float vertDelta, float horizDelta)
{
	_pos += (_right * horizDelta);
	_pos += (_up * vertDelta);
}

void ed_camera::rot_x(float deltaX)
{
	_rotX += deltaX;

	// Update camera
	if (_rotX > 89.0)
		_rotX = 89;
	if (_rotX < -89.0)
		_rotX = -89;

	// Calc new direction (note conversion from degrees to radians)
	_dir[0] = (float)(cos(_rotX * PI / 180.0f) * sin(_rotY * PI / 180.0f));
	_dir[1] = (float)sin(_rotX * PI / 180.0f);
	_dir[2] = (float)(cos(_rotX * PI / 180.0f) * cos(_rotY * PI / 180.0f));

	// Update cam right vector
	_right = glm::cross(_dir, _up);
}

void ed_camera::rot_y(float deltaY)
{
	_rotY += deltaY;

	// Update camera
	if (_rotY > 360)
		_rotY = 0;
	if (_rotY < 0)
		_rotY = 360;

	// Calc new direction (note conversion from degrees to radians)
	_dir[0] = (float)(cos(_rotX * PI / 180.0f) * sin(_rotY * PI / 180.0f));
	_dir[1] = (float)sin(_rotX * PI / 180.0f);
	_dir[2] = (float)(cos(_rotX * PI / 180.0f) * cos(_rotY * PI / 180.0f));

	// Update cam right vector
	_right = glm::cross(_dir, _up);
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
