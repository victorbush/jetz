/*=============================================================================
vlk.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include <stdexcept>

#include "jetz/main/log.h"
#include "thirdparty/fmt/include/fmt/core.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
DECLARATIONS
=============================================================================*/

static const char* get_log_level_str(log_level level);

/*=============================================================================
PUBLIC STATIC VARIABLES
=============================================================================*/

static log the_logger = log();
log& log::logger = the_logger;

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void log::log_msg(log_level level, const std::string& in_msg)
{
	/* Build message */
	auto msg = fmt::format("[{}] {}\n", get_log_level_str(level), in_msg);
	
	/* Get the array of target callbacks for the log level */
	const auto& target_list = targets[level];

	/* Log the message to each target callback */
	for (auto target : target_list)
	{
		target(msg);
	}

	/* Assert on fatal log */
	if (level == log_level::FATAL)
	{
		throw new std::runtime_error("Fatal error.");
	}
}

void log::register_target(log_target_func fn)
{
	for (auto& target_list : targets)
	{
		target_list.push_back(fn);
	}
}

/*=============================================================================
STATIC FUNCTIONS
=============================================================================*/

static const char* get_log_level_str(log_level level)
{
	switch (level)
	{
	case log_level::DEBUG:
		return "DEBUG";
	case log_level::INFO:
		return "INFO";
	case log_level::WARN:
		return "WARN";
	case log_level::ERROR:
		return "ERROR";
	case log_level::FATAL:
		return "FATAL";
	}

	/* Unknown log level */
	return "";
}

}   /* namespace jetz */
