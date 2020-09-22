/*=============================================================================
ecs_loader_system.h

The loader system handles asset loading (like loading models and getting them
into the GPU). The loader singleton component is populated with assets that
need loaded, and the loader system takes the appropriate steps to load them.
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/gpu/gpu.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ecs_loader_system {

public:

	ecs_loader_system();
	~ecs_loader_system();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	void run(ecs& ecs, gpu& gpu);

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

};

}   /* namespace jetz */
