#pragma once

#define SOLUTION_DIR	__FILE__"/../.."

/*-----------------------------------------------------
Lua
-----------------------------------------------------*/
#if _MSC_VER >= 1910
	#pragma comment (lib, SOLUTION_DIR"/thirdparty/lua/vc15/lua53.lib")
#else
	#error Need to add Lua libraries for other Visual Studio versions.
#endif
