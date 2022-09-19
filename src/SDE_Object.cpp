#include "SDE_Object.h"

#include "SDE_PackageTool.h"

#include <lua.hpp>

SDE_Object* SDE_CheckObject(lua_State* pLuaVM, int nIndex, const char* strObjectType)
{
	SDE_Object* pObject = (SDE_Object*)lua_touserdata(pLuaVM, nIndex);

	if (!pObject || pObject->GetType() != strObjectType)
		return nullptr;

	return pObject;
}