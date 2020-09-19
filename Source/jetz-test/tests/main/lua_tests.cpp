/*=============================================================================
lua_tests.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/main/lua.h"
#include "thirdparty/google_test/google_test.h"

/*=============================================================================
HELPERS
=============================================================================*/

void verify_pair(bool success, float value, jetz::lua_result<float> pair)
{
	EXPECT_EQ(success, pair.valid);	
	EXPECT_EQ(value, pair.value);
}

void verify_pair(bool success, int value, jetz::lua_result<int> pair)
{
	EXPECT_EQ(success, pair.valid);
	EXPECT_EQ(value, pair.value);
}

void verify_pair(bool success, std::string value, jetz::lua_result<std::string> pair)
{
	EXPECT_EQ(success, pair.valid);
	EXPECT_EQ(value, pair.value);
}

void verify_pair(bool success, bool value, jetz::lua_result<bool> pair)
{
	EXPECT_EQ(success, pair.valid);
	EXPECT_EQ(value, pair.value);
}

/*=============================================================================
TESTS
=============================================================================*/

/*-----------------------------------------------------
CancelLoop()
-----------------------------------------------------*/

TEST(LuaTests, CancelLoop_LoopInProgress_TrueReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11, [2] = 12 }");
	s.Push("table");
	s.StartLoop();
	s.Next();
	EXPECT_TRUE(s.CancelLoop());
	EXPECT_FALSE(s.Next());
}

TEST(LuaTests, CancelLoop_StackEmpty_FalseReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11, [2] = 12 }");
	s.Push("table");
	EXPECT_FALSE(s.CancelLoop());
	verify_pair(false, 0, s.GetInt());
}

/*-----------------------------------------------------
GetBool()
-----------------------------------------------------*/

TEST(LuaTests, GetBool_GlobalVariable_BoolReturned)
{
	jetz::lua s;
	s.ExecuteString("falseVar = false");
	s.ExecuteString("trueVar = true");

	/* Returns <success, value> */
	verify_pair(true, false, s.GetBool("falseVar"));
	verify_pair(true, true, s.GetBool("trueVar"));
}

/*
table.nestedTable.key
*/
TEST(LuaTests, GetBool_NestedTable_BoolReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { nestedTable = { key = true } }");
	verify_pair(true, true, s.GetBool("table.nestedTable.key"));
}

TEST(LuaTests, GetBool_NoScript_FalseReturned)
{
	jetz::lua s;
	verify_pair(false, false, s.GetBool("myvar"));
}

/*
table.key
*/
TEST(LuaTests, GetBool_TableDot_BoolReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = true, key2 = true }");

	verify_pair(true, true, s.GetBool("table.key1"));
	verify_pair(true, true, s.GetBool("table.key2"));
}

TEST(LuaTests, GetBool_Top_BoolReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = true, key2 = true }");
	s.Push("table.key1");
	verify_pair(true, true, s.GetBool());
}

TEST(LuaTests, GetBool_TopEmpty_FalseReturned)
{
	jetz::lua s;
	verify_pair(false, false, s.GetBool());
}

/*-----------------------------------------------------
GetFloat()
-----------------------------------------------------*/

TEST(LuaTests, GetFloat_GlobalVariable_FloatReturned)
{
	jetz::lua s;
	s.ExecuteString("posVar = 2.1");
	s.ExecuteString("negVar = -3.2");
	s.ExecuteString("zeroVar = 0.0");
	verify_pair(true, 2.1f, s.GetFloat("posVar"));
	verify_pair(true, -3.2f, s.GetFloat("negVar"));
	verify_pair(true, 0.0f, s.GetFloat("zeroVar"));
}

/*
table.nestedTable.key
*/
TEST(LuaTests, GetFloat_NestedTable_FloatReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { nestedTable = { key = 2.42312 } }");
	verify_pair(true, 2.42312f, s.GetFloat("table.nestedTable.key"));
}

TEST(LuaTests, GetFloat_NoScript_ZeroReturned)
{
	jetz::lua s;
	verify_pair(false, 0.0f, s.GetFloat("myvar"));
}

/*
table.key
*/
TEST(LuaTests, GetFloat_TableDot_FloatReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 4.34, key2 = 1.23 }");
	verify_pair(true, 4.34f, s.GetFloat("table.key1"));
	verify_pair(true, 1.23f, s.GetFloat("table.key2"));
}

TEST(LuaTests, GetFloat_Top_FloatReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 4.34, key2 = 1.23 }");
	s.Push("table.key1");
	verify_pair(true, 4.34f, s.GetFloat());
}

TEST(LuaTests, GetFloat_TopEmpty_ZeroReturned)
{
	jetz::lua s;
	verify_pair(false, 0.0f, s.GetFloat());
}

/*-----------------------------------------------------
GetFloatArray()
-----------------------------------------------------*/

TEST(LuaTests, GetFloatArray_Valid_ArrayReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { 10.2, 11.3, 12.4 }");
	s.Push("table");

	auto ret = s.GetArray<float, 3>();
	EXPECT_TRUE(ret.valid);
	EXPECT_EQ(10.2f, ret.value[0]);
	EXPECT_EQ(11.3f, ret.value[1]);
	EXPECT_EQ(12.4f, ret.value[2]);
}

/*-----------------------------------------------------
GetInt()
-----------------------------------------------------*/

TEST(LuaTests, GetInt_GlobalVariable_IntReturned)
{
	jetz::lua s;
	s.ExecuteString("posVar = 2");
	s.ExecuteString("negVar = -3");
	s.ExecuteString("zeroVar = 0");
	verify_pair(true, 2, s.GetInt("posVar"));
	verify_pair(true, -3, s.GetInt("negVar"));
	verify_pair(true, 0, s.GetInt("zeroVar"));
}

/*
table.nestedTable.key
*/
TEST(LuaTests, GetInt_NestedTable_IntReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { nestedTable = { key = 2312 } }");
	verify_pair(true, 2312, s.GetInt("table.nestedTable.key"));
}

TEST(LuaTests, GetInt_NoScript_ZeroReturned)
{
	jetz::lua s;
	verify_pair(false, 0, s.GetInt("myvar"));
}

/*
table.key
*/
TEST(LuaTests, GetInt_TableDot_IntReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 2, key2 = 5 }");
	verify_pair(true, 2, s.GetInt("table.key1"));
	verify_pair(true, 5, s.GetInt("table.key2"));
}

TEST(LuaTests, GetInt_Top_IntReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 2, key2 = 5 }");
	s.Push("table.key1");
	verify_pair(true, 2, s.GetInt());
}

TEST(LuaTests, GetInt_TopEmpty_ZeroReturned)
{
	jetz::lua s;
	verify_pair(false, 0, s.GetInt());
}

/*-----------------------------------------------------
GetKey()
-----------------------------------------------------*/

TEST(LuaTests, GetKey_OnlyOnePushed_Fail)
{
	jetz::lua s;
	s.ExecuteString("emptyStr = \"\"");
	s.Push("emptyStr");
	verify_pair(false, "", s.GetKey());
}

TEST(LuaTests, GetKey_StackEmpty_Fail)
{
	jetz::lua s;
	verify_pair(false, "", s.GetKey());
}

TEST(LuaTests, GetKey_TwoPushed_Success)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11,  [2] = 12, [3] = 13 }");
	s.Push("table");

	/* NOTE: Using indices in the table since key ordering is not reliable */

	s.StartLoop();
	s.Next();
	verify_pair(true, "0", s.GetKey());
	
	s.Next();
	verify_pair(true, "1", s.GetKey());

	s.Next();
	verify_pair(true, "2", s.GetKey());
}

/*-----------------------------------------------------
GetString()
-----------------------------------------------------*/

TEST(LuaTests, GetString_GlobalVariable_StringReturned)
{
	jetz::lua s;
	s.ExecuteString("emptyStr = \"\"");
	s.ExecuteString("str = \"Hello, world!\"");
	verify_pair(true, "", s.GetString("emptyStr"));
	verify_pair(true, "Hello, world!", s.GetString("str"));
}

/*
table.nestedTable.key
*/
TEST(LuaTests, GetString_NestedTable_StringReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { nestedTable = { key = \"Hello, world!\" } }");
	verify_pair(true, "Hello, world!", s.GetString("table.nestedTable.key"));
}

TEST(LuaTests, GetString_NoScript_EmptyStringReturned)
{
	jetz::lua s;
	verify_pair(false, "", s.GetString("myvar"));
}

/*
table.key
*/
TEST(LuaTests, GetString_TableDot_StringReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = \"String 1\", key2 = \"String 2\" }");
	verify_pair(true, "String 1", s.GetString("table.key1"));
	verify_pair(true, "String 2", s.GetString("table.key2"));
}

TEST(LuaTests, GetString_Top_StringReturned)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = \"String 1\", key2 = \"String 2\" }");
	s.Push("table.key1");
	verify_pair(true, "String 1", s.GetString());
}

TEST(LuaTests, GetString_TopEmpty_EmptyStringReturned)
{
	jetz::lua s;
	verify_pair(false, "", s.GetString());
}

/*-----------------------------------------------------
Next()
-----------------------------------------------------*/

TEST(LuaTests, Next_EmptyStack_NothingPushed)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11, [2] = 12 }");
	EXPECT_FALSE(s.Next());
	verify_pair(false, 0, s.GetInt());
}

/*-----------------------------------------------------
Pop()
-----------------------------------------------------*/

TEST(LuaTests, Pop_NothingToPop_NotPopped)
{
	jetz::lua s;
	EXPECT_EQ(0, s.Pop(1));
}

TEST(LuaTests, Pop_TryToPopMoreThanAble_ValidPopped)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 1, key2 = 2 }");
	s.Push("table");
	EXPECT_EQ(1, s.Pop(5));
}

TEST(LuaTests, Pop_SomethingToPop_Popped)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 1, key2 = 2 }");
	s.Push("table");
	EXPECT_EQ(1, s.Pop(1));
}

/*-----------------------------------------------------
PopAll()
-----------------------------------------------------*/

TEST(LuaTests, PopAll_EmptyStack_NothingPopped)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 1, key2 = 2 }");
	EXPECT_EQ(0, s.PopAll());
}

TEST(LuaTests, PopAll_PopulatedStack_AllPopped)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 1, key2 = 2 }");
	s.Push("table.key1");
	EXPECT_EQ(2, s.PopAll());
}

/*-----------------------------------------------------
Push()
-----------------------------------------------------*/

TEST(LuaTests, Push_GlobalTable_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 1, key2 = 2 }");
	
	EXPECT_EQ(1, s.Push("table"));
	verify_pair(true, 1, s.GetInt("key1"));
	verify_pair(true, 2, s.GetInt("key2"));
}

TEST(LuaTests, Push_GlobalVar_Pushed)
{
	jetz::lua s;
	s.ExecuteString("val = 1");
	EXPECT_EQ(1, s.Push("val"));
}

TEST(LuaTests, Push_NestedTableOnePush_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { nest = { val = 2.3 } }");

	EXPECT_EQ(1, s.Push("table"));
	EXPECT_EQ(1, s.Push("nest"));
	verify_pair(true, 2.3f, s.GetFloat("val"));
}

TEST(LuaTests, Push_NestedTableTwoPush_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { nest = { val = 2.3 } }");

	EXPECT_EQ(2, s.Push("table.nest"));
	verify_pair(true, 2.3f, s.GetFloat("val"));
}

TEST(LuaTests, Push_NestedVar_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { nest = { val = 2.3 } }");
	EXPECT_EQ(3, s.Push("table.nest.val"));
}

TEST(LuaTests, Push_NotFound_NotPushed)
{
	jetz::lua s;
	s.ExecuteString("table = { nest = { val = 2.3 } }");
	EXPECT_EQ(0, s.Push("notfound"));
}

TEST(LuaTests, Push_NotFoundNested_NotPushed)
{
	jetz::lua s;
	s.ExecuteString("table = { nest = { val = 2.3 } }");
	EXPECT_EQ(0, s.Push("table.notfound"));
}

/*-----------------------------------------------------
StartLoop()
-----------------------------------------------------*/

TEST(LuaTests, StartLoop_EmptyStack_NothingPushed)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11, [2] = 12 }");
	EXPECT_FALSE(s.StartLoop());
	verify_pair(false, 0, s.GetInt());
}

/*-----------------------------------------------------
StartLoop() / Next()
-----------------------------------------------------*/

TEST(LuaTests, StartLoopNext_IndexKeysMix_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, key1 = 11, [2] = 12 }");
	EXPECT_TRUE(s.Push("table"));

	/* NOTE: When using keys in tables (instead of all indices), the ordering
				is not guaranteed. */

	EXPECT_TRUE(s.StartLoop());
	EXPECT_TRUE(s.Next());
	EXPECT_TRUE(s.Next());
	EXPECT_TRUE(s.Next());

	EXPECT_FALSE(s.Next());		/* Should be end of array */
	EXPECT_EQ(1, s.Pop(5));		/* Should only be one element left on stack */
}

TEST(LuaTests, StartLoopNext_IndexOnly_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { [0] = 10, [1] = 11, [2] = 12 }");
	EXPECT_TRUE(s.Push("table"));
	
	EXPECT_TRUE(s.StartLoop());
	EXPECT_TRUE(s.Next());
	verify_pair(true, 10, s.GetInt());

	EXPECT_TRUE(s.Next());
	verify_pair(true, 11, s.GetInt());

	EXPECT_TRUE(s.Next());
	verify_pair(true, 12, s.GetInt());

	EXPECT_FALSE(s.Next());		/* Should be end of array */
	EXPECT_EQ(1, s.Pop(5));		/* Should only be one element left on stack */
}

TEST(LuaTests, StartLoopNext_KeysOnly_Pushed)
{
	jetz::lua s;
	s.ExecuteString("table = { key1 = 10, key2 = 11, key3 = 12 }");
	EXPECT_TRUE(s.Push("table"));

	/* NOTE: When using keys in tables (instead of all indices), the ordering
				is not guaranteed. */

	EXPECT_TRUE(s.StartLoop());
	EXPECT_TRUE(s.Next());
	EXPECT_TRUE(s.Next());
	EXPECT_TRUE(s.Next());

	EXPECT_FALSE(s.Next());	/* Should be end of array */
	EXPECT_EQ(1, s.Pop(5));		/* Should only be one element left on stack */
}

/*=============================================================================
OTHER TESTS
=============================================================================*/

TEST(LuaTests, OtherTests_ForLoop_LoopExecuted)
{
	jetz::lua s;
	EXPECT_TRUE(s.ExecuteString("val = 0 \n for i = 0, 6, 1 do val = i end"));
	verify_pair(true, 6, s.GetInt("val"));
}
