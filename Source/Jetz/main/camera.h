/*=============================================================================
camera.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "jetz/main/camera.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {
	
/*=============================================================================
TYPES
=============================================================================*/

class camera {

public:

	camera();
	~camera();

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/
	glm::vec3 get_pos() const;
	glm::mat4 get_view_matrix() const;
	void move(float moveDelta);
	void pan(float vertDelta, float horizDelta);
	void rot_x(float deltaX);
	void rot_y(float deltaY);

private:

	/*-----------------------------------------------------
	Private Variables
	-----------------------------------------------------*/
	glm::vec3	_dir;
	glm::vec3	_pos;
	glm::vec3	_up;
	glm::vec3	_right;
	float		_rotX;		/* degrees rotation on x axis */
	float		_rotY;		/* degrees rotation on y axis */
};

}   /* namespace jetz */
