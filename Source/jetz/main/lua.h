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
	std::pair<bool, std::vector<T>> GetArray();
	std::pair<bool, bool>			GetBool();
	std::pair<bool, bool>			GetBool(const std::string& variable);
	std::pair<bool, float>			GetFloat();
	std::pair<bool, float>			GetFloat(const std::string &variable);
	std::pair<bool, int>			GetInt();
	std::pair<bool, int>			GetInt(const std::string &variable);
	std::pair<bool, std::string>	GetKey();
	std::pair<bool, std::string>	GetString();
	std::pair<bool, std::string>	GetString(const std::string &variable);
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

	template<class T> std::pair<bool, T>	getValue() { return T(); }
	template<> std::pair<bool, bool>		getValue() { return GetBool(); }
	template<> std::pair<bool, float>		getValue() { return GetFloat(); }
	template<> std::pair<bool, int>			getValue() { return GetInt(); }
	template<> std::pair<bool, std::string> getValue() { return GetString(); }

	template<class T> T				getDefaultValue() { return T(); }
	template<> bool					getDefaultValue() { return false; }
	template<> float				getDefaultValue() { return 0.0f; }
	template<> int					getDefaultValue() { return 0; }
	template<> std::string			getDefaultValue() { return ""; }

};

/*=============================================================================
EXPLICIT INSTANTIATIONS
=============================================================================*/

template std::pair<bool, std::vector<float>> lua::GetArray<float, 2>();
template std::pair<bool, std::vector<float>> lua::GetArray<float, 3>();
template std::pair<bool, std::vector<float>> lua::GetArray<float, 4>();

template std::pair<bool, std::vector<int>> lua::GetArray<int, 2>();
template std::pair<bool, std::vector<int>> lua::GetArray<int, 3>();
template std::pair<bool, std::vector<int>> lua::GetArray<int, 4>();

}   /* namespace jetz */
