#include "SDE_Constructor.h"

#include "SDE_PackageTool.h"
#include "SDE_State.h"
#include "SDE_Debugger.h"

#include <lua.hpp>

SDE_Constructor* SDE_ToConstructor(lua_State* pLuaVM, int nIndex)
{
	SDE_Constructor* pConstructor = nullptr;

	if (lua_isstring(pLuaVM, nIndex))
	{
		SDE_GetGlobal(pLuaVM);

		lua_pushstring(pLuaVM, SDE_GLOBAL_REGISTRY);
		lua_rawget(pLuaVM, -2);

		lua_pushvalue(pLuaVM, nIndex);
		lua_rawget(pLuaVM, -2);

		pConstructor = (SDE_Constructor*)SDE_PackageTool::ToAccessor(pLuaVM, -1, SDE_TYPE_CONSTRUCTOR);

		lua_pop(pLuaVM, 3);
	}
	else pConstructor = (SDE_Constructor*)SDE_PackageTool::ToAccessor(pLuaVM, nIndex, SDE_TYPE_CONSTRUCTOR);

	if (!pConstructor)
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to get constructor because the userdata isn't a constructor."
		);
	}

	return pConstructor;
}

SDE_Constructor* SDE_CheckConstructor(lua_State* pLuaVM, int nIndex, const char* strMetatableName)
{
	SDE_Constructor* pConstructor = SDE_ToConstructor(pLuaVM, nIndex);

	if (pConstructor && pConstructor->GetType() != strMetatableName)
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Constructor type mismatch, require %s but get %s.",
			strMetatableName, pConstructor->GetType()
		);
		pConstructor = nullptr;
	}

	return pConstructor;
}

void SDE_CallConstructor(lua_State* pLuaVM, SDE_Constructor* pConstructor, int nIndexUserdata, int nIndexTable)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
	lua_rawget(pLuaVM, -2);
	lua_rawgeti(pLuaVM, -1, pConstructor->GetFuncRef());

	if (nIndexUserdata < 0) nIndexUserdata -= 3;
	if (nIndexTable < 0) nIndexTable -= 3;

	lua_pushvalue(pLuaVM, nIndexUserdata);

	if (nIndexTable != 0)
	{
		lua_pushvalue(pLuaVM, nIndexTable);
	}
	else lua_newtable(pLuaVM);

	if (lua_pcall(pLuaVM, 2, 0, 0))
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::ERROR,
			"Failed to call constructor, %s.",
			lua_tostring(pLuaVM, -1)
		);
		lua_pop(pLuaVM, 1);
	}

	lua_pop(pLuaVM, 2);
}