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
	for (auto it = ecs.cbegin(); it != ecs.cend(); ++it)
	{
		auto ent = *it;

		if (!ecs.models.exists(ent) || !ecs.transforms.exists(ent))
		{
			continue;
		}

		auto model = ecs.models.get(ent);
		auto gpu_model = gpu.get_model(model->model_filename).lock();
		auto transform = ecs.transforms.get(ent);
		gpu_model->render(frame, *transform);
	}
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void ecs_render_system::render_shadows(ecs& ecs, gpu& gpu, gpu_frame& frame)
{
	for (auto it = ecs.cbegin(); it != ecs.cend(); ++it)
	{
		auto ent = *it;

		if (!ecs.lights.exists(ent) || !ecs.transforms.exists(ent))
		{
			continue;
		}

		auto light = ecs.lights.get(ent);
		auto transform = ecs.transforms.get(ent);

	}
}

void ecs_render_system::render_objects(ecs& ecs, gpu& gpu, gpu_frame& frame)
{
	for (auto it = ecs.cbegin(); it != ecs.cend(); ++it)
	{
		auto ent = *it;

		if (!ecs.models.exists(ent) || !ecs.transforms.exists(ent))
		{
			continue;
		}

		auto model = ecs.models.get(ent);
		auto gpu_model = gpu.get_model(model->model_filename).lock();
		auto transform = ecs.transforms.get(ent);
		gpu_model->render(frame, *transform);
	}
}

}   /* namespace jetz */
