/*=============================================================================
lua.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/lua.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

/**
Constructor
*/
lua::lua()
{
	_state = luaL_newstate();

	if (!_state)
	{
		LOG_FATAL("Could not create Lua state.");
	}

	/* Open libs */
	luaL_openlibs(_state);
}

/** 
Desctructor
*/
lua::~lua()
{
	if (_state)
	{
		lua_close(_state);
	}
}

/**
Cancels loop iteration. Must be preceeded by either StartLoop() or Next().
*/
bool lua::CancelLoop()
{
	/* The a table interation must be in progress:
		[ 0]
		[-1] -- value
		[-2] -- key
		[-3] -- table
	*/
	if (lua_gettop(_state) < 3)
	{
		/* Unexpected state */
		return false;
	}

	/* Pop the key/value and leave table */
	lua_pop(_state, 2);
	return true;
}

template <class T, size_t N>
std::pair<bool, std::vector<T>> lua::GetArray()
{
	std::vector<T> data(N);

	/* A table with N values should be on top of stack */
	if (!lua_istable(_state, -1))
	{
		LOG_ERROR("Expected table on top of stack.");
		return std::make_pair(false, data);
	}

	/* Begin iterating through table */
	if (!StartLoop())
	{
		return std::make_pair(false, data);
	}

	for (size_t i = 0; i < N; ++i)
	{
		if (!Next())
		{
			/* Expected another float value */
			LOG_ERROR("Fewer values in table than expected.");
			return std::make_pair(false, data);
		}

		/* Try to get value */
		std::pair<bool, T> d = getValue<T>();
		if (!d.first)
		{
			/* Expected a value */
			return std::make_pair(false, data);
		}

		/* Store value in output array */
		data[i] = d.second;
	}

	/* Expect this to be the end of the table */
	if (Next())
	{
		// Pop off key/value
		Pop(2);

		LOG_ERROR("More values in table than expected.");
		return std::make_pair(false, data);
	}

	/* Success */
	return std::make_pair(true, data);
}

std::pair<bool, bool> lua::GetBool()
{
	if (!lua_isboolean(_state, -1))
	{
		LOG_ERROR("Not a boolean.");
		return std::make_pair(false, false);
	}

	/* Get the value */
	bool val = (bool)lua_toboolean(_state, -1);
	return std::make_pair(true, val);
}

std::pair<bool, bool> lua::GetBool(const std::string &variable)
{
	auto pushed = Push(variable);
	if (!pushed)
	{
		return std::make_pair(false, false);
	}

	auto result = GetBool();

	Pop(pushed);
	return result;
}

std::pair<bool, float> lua::GetFloat()
{
	if (!lua_isnumber(_state, -1))
	{
		LOG_ERROR("Not a number.");
		return std::make_pair(false, 0.0f);
	}

	/* Get the value */
	float val = (float)lua_tonumber(_state, -1);
	return std::make_pair(true, val);
}

std::pair<bool, float> lua::GetFloat(const std::string &variable)
{
	auto pushed = Push(variable);
	if (!pushed)
	{
		return std::make_pair(false, 0.0f);
	}

	auto result = GetFloat();

	Pop(pushed);
	return result;
}

std::pair<bool, int> lua::GetInt()
{
	if (!lua_isnumber(_state, -1))
	{
		LOG_ERROR("Not a number.");
		return std::make_pair(false, 0);
	}

	/* Get the value */
	int val = (int)lua_tonumber(_state, -1);
	return std::make_pair(true, val);
}

std::pair<bool, int> lua::GetInt(const std::string &variable)
{
	auto pushed = Push(variable);
	if (!pushed)
	{
		return std::make_pair(false, 0);
	}

	auto result = GetInt();

	Pop(pushed);
	return result;
}

std::pair<bool, std::string> lua::GetKey()
{
	/* Need to have stack like this:
	[ 0]
	[-1] -- value
	[-2] -- key
*/
	if (lua_gettop(_state) < 2)
	{
		return std::make_pair(false, "");
	}

	/* Push key to top */
	lua_pushvalue(_state, -2);

	/* Convert to string */
	auto key = std::string(lua_tostring(_state, -1));

	/* Pop the value */
	lua_pop(_state, 1);

	return std::make_pair(true, key);
}

std::pair<bool, std::string> lua::GetString()
{
	if (!lua_isstring(_state, -1))
	{
		LOG_ERROR("Not a number.");
		return std::make_pair(false, "");
	}

	/* Get the value */
	std::string val(lua_tostring(_state, -1));
	return std::make_pair(true, val);
}

std::pair<bool, std::string> lua::GetString(const std::string &variable)
{
	auto pushed = Push(variable);
	if (!pushed)
	{
		return std::make_pair(false, "");
	}

	auto result = GetString();

	Pop(pushed);
	return result;
}

bool lua::ExecuteFile(const std::string &filePath)
{
	/* Load and execute script file */
	if (luaL_dofile(_state, filePath.c_str()))
	{
		LOG_ERROR("Failed to execute script '" + filePath + "'.");
		return false;
	}

	return true;
}

bool lua::ExecuteString(const std::string &str)
{
	/* Load and execute script from string */
	if (luaL_dostring(_state, str.c_str()))
	{
		LOG_ERROR("Failed to execute script string '" + str + "'.");
		return false;
	}

	return true;
}

/**
Pops previous value and pops previous key. If a next key is present in the table,
push the next key and then pushes the next value.

Must be preceeded by either PushTableLoop() or Next().
*/
bool lua::Next()
{
	/* Need to have stack like this:
		[ 0]
		[-1] -- previous value
		[-2] -- previous key
		[-3] -- table
	*/
	if (lua_gettop(_state) < 3)
	{
		return false;
	}

	/* Pop the previous value */
	lua_pop(_state, 1);

	/* Pops previous key, replaces with next key. Then pushes value, if available.
		Result after call if next key is available:
		[ 0]
		[-1] -- value
		[-2] -- key
		[-3] -- table

		Result after call if next key not available:
		[ 0]
		[-1] -- table
	*/
	return lua_next(_state, -2);
}

uint32_t lua::Pop(uint32_t num)
{
	int pop = static_cast<int>(num);
	int top = lua_gettop(_state);

	if (top == 0)
	{
		/* Already at top */
		return 0;
	}

	if (pop > top)
	{
		/* Don't pop more than there actually are */
		pop = top;
	}

	lua_pop(_state, pop);
	return static_cast<uint32_t>(pop);
}

uint32_t lua::PopAll()
{
	int top = lua_gettop(_state);
	lua_pop(_state, top);
	return top;
}

/**
Pushes the specified variable path onto the stack. Nested variables are supported
using the period separator.

Examples:
	variableName
	tableName
	tableName.key
	tableName.nestedTable.key

@param variable The name of the variable to push.
@return The number of elements pushed.
*/
uint32_t lua::Push(const std::string &variable)
{
	int numPushed = 0;
	std::string var = "";

	for (int i = 0; i < variable.size(); ++i)
	{
		auto isDot = variable[i] == '.';

		if (!isDot)
		{
			/* Part of variable name */
			var += variable[i];
		}

		if (isDot || (i + 1) == variable.size())
		{
			/* At the end of a variable name, push it */
			if (!pushSingle(var))
			{
				/* Variable not found */
				Pop(numPushed);
				return 0;
			}

			numPushed++;
			var = "";
		}
	}
	
	/* Success */
	return numPushed;
}

/**
Setup iteration through table. Pushes nil twice. Expects a table
to be on top of stack. Use Next() to iterate through values.
*/
bool lua::StartLoop()
{
	/* The table/array to iterate must already be on stack:
		[ 0]
		[-1] -- table
	*/
	if (lua_gettop(_state) == 0)
	{
		/* Nothing on stack, so nothing to push */
		return false;
	}

	/* Push nil as initial key - this tells lua_next() to start at beginning */
	lua_pushnil(_state);

	/* Push nil as initial value */
	lua_pushnil(_state);

	return true;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

/**
Pushes a single variable on the stack.
*/
bool lua::pushSingle(const std::string &variable)
{
	//if (lua_isnil(_state, -1))
	if (lua_gettop(_state) == 0)
	{
		/* Nothing below, find global variable */
		lua_getglobal(_state, variable.c_str());
	}
	else
	{
		/* Something already pushed, find a field */
		lua_getfield(_state, -1, variable.c_str());
	}

	/* Check if found */
	if (lua_isnil(_state, -1))
	{
		/* Not found */
		Pop(1);
		return false;
	}

	/* Success */
	return true;
}

}   /* namespace jetz */
