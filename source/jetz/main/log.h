/*=============================================================================
log.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <vector>

#include "thirdparty/fmt/include/fmt/core.h"

/*=============================================================================
MACROS
=============================================================================*/

/** Gets the name of the current source file. Extension of the __FILE__ macro. */
#define __FILENAME__ (strrchr("\\" __FILE__, '\\') + 1)

#define LOG_DBG(msg) \
	jetz::log::logger.log_msg(jetz::log_level::LOG_DEBUG, msg)

#define LOG_DBG_FMT(msg, ...) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_DEBUG, __FILENAME__, __LINE__, msg, __VA_ARGS__)

#define LOG_INFO(msg) \
	jetz::log::logger.log_msg(jetz::log_level::LOG_INFO, msg)

#define LOG_INFO_FMT(msg, ...) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_INFO, __FILENAME__, __LINE__, msg, __VA_ARGS__)

#define LOG_WARN(msg) \
	jetz::log::logger.log_msg(jetz::log_level::LOG_WARN, msg)

#define LOG_WARN_FMT(msg, ...) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_WARN, __FILENAME__, __LINE__, msg, __VA_ARGS__)

#define LOG_ERROR(msg) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_ERROR, __FILENAME__, __LINE__, msg)

#define LOG_ERROR_FMT(msg, ...) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_ERROR, __FILENAME__, __LINE__, msg, __VA_ARGS__)

#define LOG_FATAL(msg) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_FATAL, __FILENAME__, __LINE__, msg)

#define LOG_FATA_FMT(msg, ...) \
	jetz::log::logger.log_msg_with_source(jetz::log_level::LOG_FATAL, __FILENAME__, __LINE__, msg, __VA_ARGS__)

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
VARIABLES
=============================================================================*/


/*=============================================================================
TYPES
=============================================================================*/

/**
Function type for a logging target. A logger can have multiple output targets.
@param msg The message to output to the logging target.
*/
typedef void (*log_target_func)(const std::string& msg);

/**
Log levels.
*/
enum class log_level
{
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,

	_COUNT
};

/**
Logging utiltiies.
*/
class log 
{
public:

	/*-----------------------------------------------------
	Public static variables
	-----------------------------------------------------*/

	/** The global logger. Used by the LOG_XXX macros. */
	static log& logger;

	/*-----------------------------------------------------
	Public methods
	-----------------------------------------------------*/

	/**
	Logs a message.

	@param level The log level.
	@param msg The message to log.
	*/
	void log_msg(log_level level, const std::string& msg);

	/**
	Logs a message and includes the source filename and line number where the message originated from.

	@param level The log level.
	@param filename The source filename.
	@param line The line number in the source file.
	@param msg_fmt The message to log.
	@param args Arguments (if any) to be placed in the msg_fmt string.
	*/
	template<typename... Args>
	void log_msg_with_source
		(
		log_level				level,
		const std::string&		filename,
		int						line,
		const std::string&		msg_fmt,
		Args&&...				args
		)
	{
		/* Build message: [Filename:Line] message */
		std::string tmp = fmt::format(msg_fmt, args...);
		std::string msg = fmt::format("[{}:{}] {}", filename, line, tmp);

		/* Log */
		log_msg(level, msg);
	}

	/**
	Registers a logging target function for all log levels.
	*/
	void register_target(log_target_func fn);

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/**
	The logging target functions for each log level. Each log level can output
	to multiple logging targets.
	*/
	std::vector<log_target_func> targets[(int)log_level::_COUNT];
};

}   /* namespace jetz */
