#include "SDE_PackageTool.h"

#include "SDE_State.h"
#include "SDE_Constructor.h"
#include "SDE_Debugger.h"

#include <lua.hpp>
#include <string>

const char* SDE_CastObjectType(SDE_ObjectType enumType)
{
	switch (enumType)
	{
	case SDE_ObjectType::SCENE:
		return SDE_TYPE_SCENE;

	case SDE_ObjectType::SYSTEM:
		return SDE_TYPE_SYSTEM;

	case SDE_ObjectType::ENTITY:
		return SDE_TYPE_ENTITY;

	case SDE_ObjectType::COMPONENT:
		return SDE_TYPE_COMPONENT;

	default:
		return "SDE_TYPE_NULL";
	}
}

SDE_LUA_FUNC(SDE_Object_CreateConstructor)
{
	SDE_State* pState = SDE_GetState(pLuaVM);

	const char* strTypeObject = SDE_CastObjectType((SDE_ObjectType)luaL_checkinteger(pLuaVM, 1));

	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 2);
	int nRefFunc = luaL_ref(pLuaVM, -2);

	SDE_Constructor* pConstructor = pState->CreateConstructor(strTypeObject, nRefFunc);
	SDE_PackageTool::NewAccessor(pLuaVM, pConstructor, SDE_TYPE_CONSTRUCTOR);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_RegisterConstructor)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_REGISTRY);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 1);
	lua_pushvalue(pLuaVM, 2);
	lua_rawset(pLuaVM, -3);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_UnregisterConstructor)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_REGISTRY);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 1);
	lua_pushnil(pLuaVM);
	lua_rawset(pLuaVM, -3);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetConstructor)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_REGISTRY);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 1);
	lua_rawget(pLuaVM, -2);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_PreloadScene)
{
	SDE_State* pState = SDE_GetState(pLuaVM);

	SDE_Constructor* pConstructor = SDE_CheckConstructor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pConstructor)
	{
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Scene* pScene = pState->CreateScene();
	SDE_PackageTool::NewAccessor(pLuaVM, pScene, SDE_TYPE_SCENE);

	if (lua_gettop(pLuaVM) >= 3)
	{
		SDE_CallConstructor(pLuaVM, pConstructor, -1, 2);
	}
	else SDE_CallConstructor(pLuaVM, pConstructor, -1);

	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_SCENEUDATA);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, -3);
	pScene->SetUserdataRef(luaL_ref(pLuaVM, -2));

	lua_pop(pLuaVM, 2);

	pScene->Initialize();

	return 1;
}

SDE_LUA_FUNC(SDE_Object_UpdateScene)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pScene) return 0;
	pScene->Update();
	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetSceneSystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pScene) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	const char* strName = luaL_checkstring(pLuaVM, 2);
	SDE_System* pSystem = pScene->GetSystem(strName);
	if (pSystem)
	{
		lua_pushlightuserdata(pLuaVM, pSystem);
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_TraverseSceneComponent)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pScene) return 0;

	const char* strName = luaL_checkstring(pLuaVM, 2);
	pScene->TraverseComponent(
		strName,
		[&pLuaVM](SDE_Component* pComponent)
		{
			lua_pushvalue(pLuaVM, 3);
			lua_pushlightuserdata(pLuaVM, pComponent);
			
			if (lua_pcall(pLuaVM, 1, 0, 0))
			{
				SDE_Debugger::Instance().Output(
					SDE_Debugger::OutputType::ERROR,
					"Failed to call scene's component traversal function, %s.",
					lua_tostring(pLuaVM, -1)
				);
				lua_pop(pLuaVM, 1);
			}
		}
	);
	return 0;
}

SDE_LUA_FUNC(SDE_Object_TraverseEntityComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_ENTITY);
	if (!pEntity) return 0;

	pEntity->GetScene()->TraverseComponent(
		pEntity,
		[&pLuaVM](SDE_Component* pComponent)
		{
			lua_pushvalue(pLuaVM, 2);
			lua_pushlightuserdata(pLuaVM, pComponent);

			if (lua_pcall(pLuaVM, 1, 0, 0))
			{
				SDE_Debugger::Instance().Output(
					SDE_Debugger::OutputType::ERROR,
					"Failed to call entity's component traversal function, %s.",
					lua_tostring(pLuaVM, -1)
				);
				lua_pop(pLuaVM, 1);
			}
		}
	);
	return 0;
}

SDE_LUA_FUNC(SDE_Object_CreateSystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pScene) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Constructor* pConstructor = SDE_CheckConstructor(pLuaVM, 2, SDE_TYPE_SYSTEM);
	if (!pConstructor) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_System* pSystem = pScene->CreateSystem();
	lua_pushlightuserdata(pLuaVM, pSystem);

	if (lua_gettop(pLuaVM) >= 4)
	{
		SDE_CallConstructor(pLuaVM, pConstructor, -1, 3);
	}
	else SDE_CallConstructor(pLuaVM, pConstructor, -1);

	if (!pScene->RecordSystemInfo(pSystem))
	{
		if (pSystem->GetFuncRef() != LUA_NOREF)
		{
			SDE_GetSceneTable(pLuaVM, pScene);

			lua_pushstring(pLuaVM, SDE_SCENE_SYSTEM);
			lua_rawget(pLuaVM, -2);

			luaL_unref(pLuaVM, -1, pSystem->GetFuncRef());

			lua_pop(pLuaVM, 2);
		}
		pScene->DestroySystem(pSystem);
		lua_pushnil(pLuaVM);
	}
	else pSystem->SetCompleted();

	return 1;
}

SDE_LUA_FUNC(SDE_Object_DestroySystem)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	if (!pSystem) return 0;

	SDE_GetSceneTable(pLuaVM, pSystem->GetScene());

	lua_pushstring(pLuaVM, SDE_SCENE_SYSTEM);
	lua_rawget(pLuaVM, -2);

	luaL_unref(pLuaVM, -1, pSystem->GetFuncRef());

	pSystem->GetScene()->DestroySystem(pSystem);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_CreateEntity)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
	if (!pScene) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Constructor* pConstructor = SDE_CheckConstructor(pLuaVM, 2, SDE_TYPE_ENTITY);
	if (!pConstructor)
	{
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Entity* pEntity = pScene->CreateEntity();
	lua_pushlightuserdata(pLuaVM, pEntity);

	if (lua_gettop(pLuaVM) >= 4)
	{
		SDE_CallConstructor(pLuaVM, pConstructor, -1, 3);
	}
	else SDE_CallConstructor(pLuaVM, pConstructor, -1);

	pEntity->SetCompleted();

	return 1;
}

SDE_LUA_FUNC(SDE_Object_DestroyEntity)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_ENTITY);
	if (!pEntity) return 0;

	pEntity->GetScene()->TraverseComponent(
		pEntity,
		[&pLuaVM](SDE_Component* pComponent)
		{
			SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

			lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
			lua_rawget(pLuaVM, -2);

			luaL_unref(pLuaVM, -1, pComponent->GetTableRef());

			lua_pop(pLuaVM, 2);

			pComponent->GetEntity()->GetScene()->DestroyComponent(pComponent);
		}
	);
	pEntity->GetScene()->DestroyEntity(pEntity);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_CreateComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_ENTITY);
	if (!pEntity) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Constructor* pConstructor = SDE_CheckConstructor(pLuaVM, 2, SDE_TYPE_COMPONENT);
	if (!pConstructor) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_Component* pComponent = pEntity->GetScene()->CreateComponent(pEntity);
	lua_pushlightuserdata(pLuaVM, pComponent);

	if (lua_gettop(pLuaVM) >= 4)
	{
		SDE_CallConstructor(pLuaVM, pConstructor, -1, 3);
	}
	else SDE_CallConstructor(pLuaVM, pConstructor, -1);

	if (!pEntity->GetScene()->RecordComponentInfo(pComponent))
	{
		if (pComponent->GetTableRef() != LUA_NOREF)
		{
			SDE_GetSceneTable(pLuaVM, pEntity->GetScene());

			lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
			lua_rawget(pLuaVM, -2);

			luaL_unref(pLuaVM, -1, pComponent->GetTableRef());

			lua_pop(pLuaVM, 2);
		}
		pEntity->GetScene()->DestroyComponent(pComponent);
		lua_pushnil(pLuaVM);
	}
	else pComponent->SetCompleted();

	return 1;
}

SDE_LUA_FUNC(SDE_Object_DestroyComponent)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) return 0;

	SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

	lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
	lua_rawget(pLuaVM, -2);

	luaL_unref(pLuaVM, -1, pComponent->GetTableRef());

	pComponent->GetEntity()->GetScene()->DestroyComponent(pComponent);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetEntityScene)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_ENTITY);

	if (pEntity)
	{
		SDE_GetSceneUserdata(pLuaVM, pEntity->GetScene());
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetEntityComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_ENTITY);
	if (!pEntity) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	const char* strComponentType = luaL_checkstring(pLuaVM, 2);
	SDE_Component* pComponent = pEntity->GetScene()->GetComponent(pEntity, strComponentType);

	if (pComponent)
	{
		lua_pushlightuserdata(pLuaVM, pComponent);
	}
	else lua_pushnil(pLuaVM);
	
	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetSystemName)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);

	if (pSystem)
	{
		lua_pushstring(pLuaVM, pSystem->GetName());
	}
	else lua_pushnil(pLuaVM);
	
	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetSystemName)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	if (!pSystem) return 0;

	if (pSystem->IsConstructed())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"You cannot set a name for a system that has been constructed."
		);
		return 0;
	}

	const char* strName = luaL_checkstring(pLuaVM, 2);
	pSystem->SetName(strName);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetSystemScene)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);

	if (pSystem)
	{
		SDE_GetSceneUserdata(pLuaVM, pSystem->GetScene());
	}
	else lua_pushnil(pLuaVM);
	
	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetSystemFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);

	if (pSystem->GetFuncRef() != LUA_NOREF)
	{
		SDE_GetSceneTable(pLuaVM, pSystem->GetScene());

		lua_pushstring(pLuaVM, SDE_SCENE_SYSTEM);
		lua_rawget(pLuaVM, -2);

		lua_rawgeti(pLuaVM, -1, pSystem->GetFuncRef());
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetSystemFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);

	SDE_GetSceneTable(pLuaVM, pSystem->GetScene());

	lua_pushstring(pLuaVM, SDE_SCENE_SYSTEM);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 2);

	if (pSystem->GetFuncRef() != LUA_NOREF)
	{
		lua_rawseti(pLuaVM, -2, pSystem->GetFuncRef());
	}
	else pSystem->SetFuncRef(luaL_ref(pLuaVM, -2));

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetSystemRunningStatus)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	lua_pushboolean(pLuaVM, pSystem->IsRunning());
	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetSystemRunningStatus)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	pSystem->SetRunningStatus(lua_toboolean(pLuaVM, 2));
	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetSystemType)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	lua_pushinteger(pLuaVM, (lua_Integer)pSystem->GetType());
	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetSystemType)
{
	SDE_System* pSystem = (SDE_System*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_SYSTEM);
	SDE_SystemType enumType = (SDE_SystemType)luaL_checkinteger(pLuaVM, 2);

	pSystem->SetType(enumType);

	switch (enumType)
	{
	case SDE_SystemType::NONE:
		if (pSystem->GetType() == SDE_SystemType::INITIALIZER)
		{
			pSystem->GetScene()->DeleteInitializer(pSystem);
		}
		else if (pSystem->GetType() == SDE_SystemType::UPDATER)
		{
			pSystem->GetScene()->DeleteUpdater(pSystem);
		}
		else if (pSystem->GetType() == SDE_SystemType::CONNECTOR)
		{
			if (pSystem->GetNextSystem())
			{
				pSystem->GetNextSystem()->SetPrevSystem(pSystem->GetPrevSystem());
			}
			pSystem->GetPrevSystem()->SetNextSystem(pSystem->GetNextSystem());
		}
		break;

	case SDE_SystemType::INITIALIZER:
		if (pSystem->GetType() == SDE_SystemType::UPDATER)
		{
			pSystem->GetScene()->DeleteUpdater(pSystem);
		}
		else if (pSystem->GetType() == SDE_SystemType::CONNECTOR)
		{
			if (pSystem->GetNextSystem())
			{
				pSystem->GetNextSystem()->SetPrevSystem(pSystem->GetPrevSystem());
			}
			pSystem->GetPrevSystem()->SetNextSystem(pSystem->GetNextSystem());
		}
		pSystem->GetScene()->AddInitializer(pSystem);
		break;

	case SDE_SystemType::UPDATER:
		if (pSystem->GetType() == SDE_SystemType::INITIALIZER)
		{
			pSystem->GetScene()->DeleteInitializer(pSystem);
		}
		else if (pSystem->GetType() == SDE_SystemType::CONNECTOR)
		{
			if (pSystem->GetNextSystem())
			{
				pSystem->GetNextSystem()->SetPrevSystem(pSystem->GetPrevSystem());
			}
			pSystem->GetPrevSystem()->SetNextSystem(pSystem->GetNextSystem());
		}
		pSystem->GetScene()->AddUpdater(pSystem);
		break;

	case SDE_SystemType::CONNECTOR:
		SDE_System* pSystemPrev = (SDE_System*)SDE_CheckObject(pLuaVM, 3, SDE_TYPE_SYSTEM);
		if (!pSystemPrev)
			return 0;

		pSystem->SetPrevSystem(pSystemPrev);
		pSystemPrev->SetNextSystem(pSystem);
		break;
	}
	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetComponentEntity)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);

	if (pComponent)
	{
		lua_pushlightuserdata(pLuaVM, pComponent->GetEntity());
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetComponentScene)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);

	if (pComponent)
	{
		SDE_GetSceneUserdata(pLuaVM, pComponent->GetEntity()->GetScene());
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetComponentName)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	if (pComponent->GetName())
	{
		lua_pushstring(pLuaVM, pComponent->GetName());
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetComponentName)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) return 0;

	if (pComponent->IsConstructed())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"You cannot set a name for a component that has been constructed."
		);
		return 0;
	}

	const char* strName = luaL_checkstring(pLuaVM, 2);
	pComponent->SetName(strName);

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetComponentTable)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

	lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
	lua_rawget(pLuaVM, -2);

	lua_rawgeti(pLuaVM, -1, pComponent->GetTableRef());

	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetComponentTable)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) return 0;

	SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

	lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 2);

	if (pComponent->GetTableRef() == LUA_NOREF)
	{
		pComponent->SetTableRef(luaL_ref(pLuaVM, -2));
	}
	else lua_rawseti(pLuaVM, -2, pComponent->GetTableRef());

	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetComponentValue)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) {
		lua_pushnil(pLuaVM);
		return 1;
	}

	if (pComponent->GetTableRef() != LUA_NOREF)
	{
		SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

		lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
		lua_rawget(pLuaVM, -2);

		lua_rawgeti(pLuaVM, -1, pComponent->GetTableRef());

		lua_pushvalue(pLuaVM, 2);
		lua_rawget(pLuaVM, -2);
	}
	else
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to get component's value, because you haven't set a table for this component."
		);
		lua_pushnil(pLuaVM);
	}
	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetComponentValue)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	if (!pComponent) return 0;

	if (pComponent->GetTableRef() != LUA_NOREF)
	{
		SDE_GetSceneTable(pLuaVM, pComponent->GetEntity()->GetScene());

		lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
		lua_rawget(pLuaVM, -2);

		lua_rawgeti(pLuaVM, -1, pComponent->GetTableRef());

		lua_pushvalue(pLuaVM, 2);
		lua_pushvalue(pLuaVM, 3);
		lua_rawset(pLuaVM, -3);
	}
	else
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to set component's value, because you haven't set a table for this component."
		);
	}
	return 0;
}

SDE_LUA_FUNC(SDE_Object_GetComponentSibling)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_CheckObject(pLuaVM, 1, SDE_TYPE_COMPONENT);
	const char* strSiblingName = luaL_checkstring(pLuaVM, 2);

	SDE_Scene* pScene = pComponent->GetEntity()->GetScene();
	if (pScene->GetComponent(pComponent->GetEntity(), strSiblingName))
	{
		lua_pushlightuserdata(pLuaVM, pScene->GetComponent(pComponent->GetEntity(), strSiblingName));
	}
	else lua_pushnil(pLuaVM);

	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetConstructorType)
{
	SDE_Constructor* pConstructor = (SDE_Constructor*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_CONSTRUCTOR);
	lua_pushstring(pLuaVM, pConstructor->GetType());
	return 1;
}

SDE_LUA_FUNC(SDE_Object_GetConstructorFunc)
{
	SDE_Constructor* pConstructor = (SDE_Constructor*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_CONSTRUCTOR);

	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
	lua_rawget(pLuaVM, -2);

	lua_rawgeti(pLuaVM, -1, pConstructor->GetFuncRef());

	return 1;
}

SDE_LUA_FUNC(SDE_Object_SetConstructorFunc)
{
	SDE_Constructor* pConstructor = (SDE_Constructor*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_CONSTRUCTOR);

	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
	lua_rawget(pLuaVM, -2);

	lua_pushvalue(pLuaVM, 2);
	lua_rawseti(pLuaVM, -2, pConstructor->GetFuncRef());

	return 0;
}

static SDE_PackageTool::LuaPackage s_packageObject =
{
	"SDE",
	{
		{ "CreateConstructor",			SDE_Object_CreateConstructor },
		{ "RegisterConstructor",		SDE_Object_RegisterConstructor },
		{ "UnregisterConstructor",		SDE_Object_UnregisterConstructor },
		{ "GetConstructor",				SDE_Object_GetConstructor },

		{ "GetConstructorType",			SDE_Object_GetConstructorType },
		{ "GetConstructorFunc",			SDE_Object_GetConstructorFunc },
		{ "SetConstructorFunc",			SDE_Object_SetConstructorFunc },

		{ "PreloadScene",				SDE_Object_PreloadScene },
		{ "UpdateScene",				SDE_Object_UpdateScene },

		{ "CreateSystem",				SDE_Object_CreateSystem },
		{ "DestroySystem",				SDE_Object_DestroySystem },
		{ "GetSceneSystem",				SDE_Object_GetSceneSystem },
		{ "CreateEntity",				SDE_Object_CreateEntity },
		{ "DestroyEntity",				SDE_Object_DestroyEntity },
		{ "TraverseSceneComponent",		SDE_Object_TraverseSceneComponent },

		{ "GetSystemScene",				SDE_Object_GetSystemScene },
		{ "GetSystemName",				SDE_Object_GetSystemName },
		{ "SetSystemName",				SDE_Object_SetSystemName },
		{ "GetSystemType",				SDE_Object_GetSystemType },
		{ "SetSystemType",				SDE_Object_SetSystemType },
		{ "GetSystemFunc",				SDE_Object_GetSystemFunc },
		{ "SetSystemFunc",				SDE_Object_SetSystemFunc },
		{ "GetSystemRunningStatus",		SDE_Object_GetSystemRunningStatus },
		{ "SetSystemRunningStatus",		SDE_Object_SetSystemRunningStatus },

		{ "GetEntityScene",				SDE_Object_GetEntityScene },
		{ "GetEntityComponent",			SDE_Object_GetEntityComponent },
		{ "CreateComponent",			SDE_Object_CreateComponent },
		{ "DestroyComponent",			SDE_Object_DestroyComponent },
		{ "TraverseEntityComponent",	SDE_Object_TraverseEntityComponent },

		{ "GetComponentEntity",			SDE_Object_GetComponentEntity },
		{ "GetComponentScene",		 	SDE_Object_GetComponentScene },
		{ "GetComponentSibling",		SDE_Object_GetComponentSibling },
		{ "GetComponentName",			SDE_Object_GetComponentName },
		{ "SetComponentName",			SDE_Object_SetComponentName },
		{ "GetComponentTable",			SDE_Object_GetComponentTable },
		{ "SetComponentTable",			SDE_Object_SetComponentTable },
		{ "GetComponentValue",			SDE_Object_GetComponentValue },
		{ "SetComponentValue",			SDE_Object_SetComponentValue }
	},
	{
		{ "TYPE_OBJECT_SCENE",			(lua_Integer)SDE_ObjectType::SCENE },
		{ "TYPE_OBJECT_SYSTEM",			(lua_Integer)SDE_ObjectType::SYSTEM },
		{ "TYPE_OBJECT_ENTITY",			(lua_Integer)SDE_ObjectType::ENTITY },
		{ "TYPE_OBJECT_COMPONENT",		(lua_Integer)SDE_ObjectType::COMPONENT },

		{ "TYPE_SYSTEM_NONE",			(lua_Integer)SDE_SystemType::NONE },
		{ "TYPE_SYSTEM_INITIALIZER",	(lua_Integer)SDE_SystemType::INITIALIZER },
		{ "TYPE_SYSTEM_UPDATER",		(lua_Integer)SDE_SystemType::UPDATER },
		{ "TYPE_SYSTEM_CONNECTOR",		(lua_Integer)SDE_SystemType::CONNECTOR }
	},
	{
		{
			SDE_TYPE_CONSTRUCTOR,
			{},

			[](lua_State* pLuaVM)->int
			{
				SDE_State* pState = SDE_GetState(pLuaVM);
				SDE_Constructor* pConstructor = SDE_ToConstructor(pLuaVM, 1);

				lua_pushstring(pLuaVM, SDE_GLOBAL);
				lua_rawget(pLuaVM, LUA_REGISTRYINDEX);

				lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
				lua_rawget(pLuaVM, -2);

				luaL_unref(pLuaVM, -1, pConstructor->GetFuncRef());

				pState->DestroyConstructor(pConstructor);

				return 0;
			}
		},
		{
			SDE_TYPE_SCENE,
			{},

			[](lua_State* pLuaVM)->int
			{
				SDE_State* pState = SDE_GetState(pLuaVM);

				SDE_Scene* pScene = (SDE_Scene*)SDE_PackageTool::ToAccessor(pLuaVM, 1, SDE_TYPE_SCENE);
				pState->DestroyScene(pScene);

				return 0;
			}
		}
	},
	[](lua_State* pLuaVM)->void
	{
		SDE_State* pState = new SDE_State(pLuaVM);

		lua_pushstring(pLuaVM, SDE_GLOBAL);
		lua_createtable(pLuaVM, 0, 5);
		{
			lua_pushstring(pLuaVM, SDE_GLOBAL_STATE);
			lua_pushlightuserdata(pLuaVM, pState);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_GLOBAL_REGISTRY);
			lua_newtable(pLuaVM);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_GLOBAL_CONSTRUCTOR);
			lua_newtable(pLuaVM);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_GLOBAL_SCENE);
			lua_newtable(pLuaVM);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_GLOBAL_SCENEUDATA);
			lua_newtable(pLuaVM);
			{
				lua_createtable(pLuaVM, 0, 1);

				lua_pushstring(pLuaVM, "__mode");
				lua_pushstring(pLuaVM, "v");
				lua_rawset(pLuaVM, -3);

				lua_setmetatable(pLuaVM, -2);
			}
			lua_rawset(pLuaVM, -3);
		}
		lua_rawset(pLuaVM, LUA_REGISTRYINDEX);
	},
	[](lua_State* pLuaVM)->int
	{
		SDE_GetGlobal(pLuaVM);

		lua_pushstring(pLuaVM, SDE_GLOBAL_STATE);
		lua_rawget(pLuaVM, -2);

		SDE_State* pState = (SDE_State*)lua_touserdata(pLuaVM, -1);
		delete pState;

		return 0;
	}
};

SDE_PACKAGE_MAIN(SDE)
{
	lua_createtable(pLuaVM, 0, (int)s_packageObject.GetSize());
	SDE_PackageTool::SetLuaPackage(pLuaVM, s_packageObject);
	return 1;
}