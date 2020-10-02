/*=============================================================================
ecs_render_system.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/ecs/ecs.h"
#include "jetz/ecs/systems/ecs_render_system.h"
#include "jetz/gpu/gpu.h"
#include "jetz/gpu/gpu_frame.h"
#include "jetz/gpu/gpu_model.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ecs_render_system::ecs_render_system()
{
}

ecs_render_system::~ecs_render_system()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void ecs_render_system::run(ecs& ecs, gpu& gpu, gpu_frame& frame)
{
	// TODO -- TEMP
	if (ecs.entity_exists(0))
	{
		auto m = ecs.models.get(0);
		auto model = gpu.get_model(m->model_filename).lock();
		auto transform = ecs.transforms.get((entity_id)0);
		model->render(frame, *transform);
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
