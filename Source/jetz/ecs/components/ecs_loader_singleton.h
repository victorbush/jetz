/*=============================================================================
ecs_loader_singleton.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <unordered_set>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/**
A singleton component that tracks assets that need loaded (like textures,
models, etc).
*/
class ecs_loader_singleton {
public:
	std::unordered_set<std::string> models;
};

}   /* namespace jetz */
