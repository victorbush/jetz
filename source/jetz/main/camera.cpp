/*=============================================================================
camera.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/camera.h"

/*=============================================================================
MACROS / CONSTANTS
=============================================================================*/

#define PI		glm::pi<float>()

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

camera::camera()
{
	_pos = { 0, 0, 5 };
	_right = { 1, 0, 0 };
	_up = { 0, 1, 0 };
	_dir = { 0, 0, -1 };
	_rotX = 0.0f;
	_rotY = 180.0f;
}

camera::~camera()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

glm::vec3 camera::get_pos() const
{
	return _pos;
}

glm::mat4 camera::get_view_matrix() const
{
	glm::vec3 lookAt = _pos + _dir;
	return glm::lookAt(_pos, lookAt, _up);
}

void camera::move(float moveDelta)
{
	// Movement only occurs on the X / Z axes. Kill movement on Y axis.
	glm::vec3 deltaVector = { _dir[0], 0, _dir[2] };
	deltaVector *= moveDelta;
	_pos += deltaVector;
}

void camera::pan(float vertDelta, float horizDelta)
{
	_pos += (_right * horizDelta);
	_pos += (_up * vertDelta);
}

void camera::rot_x(float deltaX)
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

void camera::rot_y(float deltaY)
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
