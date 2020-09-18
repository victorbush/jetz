/*=============================================================================
lua.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <memory>
#include <string>
#include <vector>

#include "jetz/main/log.h"
#include "thirdparty/lua/lua.hpp"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CLASS
=============================================================================*/

template <typename T>
struct lua_result
{
	lua_result(bool valid, T value) 
		:
		valid(valid),
		value(value)
	{}

	bool valid;
	T value;
};

class lua
{
public:

	lua();
	~lua();

    /*-----------------------------------------------------
    Public Methods
    -----------------------------------------------------*/
	bool							CancelLoop();
	bool							ExecuteFile(const std::string &filePath);
	bool							ExecuteString(const std::string &str);
	template <class T, size_t N>
	lua_result<std::vector<T>>		GetArray();
	lua_result<bool>				GetBool();
	lua_result<bool>				GetBool(const std::string& variable);
	lua_result<float>				GetFloat();
	lua_result<float>				GetFloat(const std::string &variable);
	lua_result<int>					GetInt();
	lua_result<int>					GetInt(const std::string &variable);
	lua_result<std::string>			GetKey();
	lua_result<std::string>			GetString();
	lua_result<std::string>			GetString(const std::string &variable);
	bool							Next();
	uint32_t						Pop(uint32_t num);
	uint32_t						PopAll();
	uint32_t						Push(const std::string &variable);
	bool							StartLoop();

private:
	
    /*-----------------------------------------------------
    Private Variables
    -----------------------------------------------------*/
	lua_State						*_state;

	/*-----------------------------------------------------
	Private Methods
	-----------------------------------------------------*/
	bool							pushSingle(const std::string &variable);

	template<class T> lua_result<T>		getValue() { return T(); }
	template<> lua_result<bool>			getValue() { return GetBool(); }
	template<> lua_result<float>		getValue() { return GetFloat(); }
	template<> lua_result<int>			getValue() { return GetInt(); }
	template<> lua_result<std::string>	getValue() { return GetString(); }

	template<class T> T				getDefaultValue() { return T(); }
	template<> bool					getDefaultValue() { return false; }
	template<> float				getDefaultValue() { return 0.0f; }
	template<> int					getDefaultValue() { return 0; }
	template<> std::string			getDefaultValue() { return ""; }

};

/*=============================================================================
EXPLICIT INSTANTIATIONS
=============================================================================*/

template lua_result<std::vector<float>> lua::GetArray<float, 2>();
template lua_result<std::vector<float>> lua::GetArray<float, 3>();
template lua_result<std::vector<float>> lua::GetArray<float, 4>();

template lua_result<std::vector<int>> lua::GetArray<int, 2>();
template lua_result<std::vector<int>> lua::GetArray<int, 3>();
template lua_result<std::vector<int>> lua::GetArray<int, 4>();

}   /* namespace jetz */
