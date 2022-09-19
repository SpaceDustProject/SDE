#include "SDE_State.h"

#include "SDE_PackageTool.h"
#include "SDE_MemoryPool.h"

#include <unordered_map>
#include <lua.hpp>

class SDE_State::Impl
{
public:
	lua_State*		m_pLuaVM;
	SDE_MemoryPool	m_memoryPool;

public:
	Impl(lua_State* pLuaVM)
	{
		m_pLuaVM = pLuaVM;
	}
};

SDE_Constructor* SDE_State::CreateConstructor(const char* strType, int nRefFunc)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_Constructor));
	SDE_Constructor* pConstructor = new (pMem) SDE_Constructor(strType, nRefFunc);
	return pConstructor;
}

void SDE_State::DestroyConstructor(SDE_Constructor* pConstructor)
{
	pConstructor->~SDE_Constructor();
	m_pImpl->m_memoryPool.Free(pConstructor, sizeof(SDE_Constructor));
}

SDE_Scene* SDE_State::CreateScene()
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_Scene));
	SDE_Scene* pScene = new (pMem) SDE_Scene(m_pImpl->m_pLuaVM);
	return pScene;
}

void SDE_State::DestroyScene(SDE_Scene* pScene)
{
	pScene->~SDE_Scene();
	m_pImpl->m_memoryPool.Free(pScene, sizeof(SDE_Scene));
}

SDE_State::SDE_State(lua_State* pLuaVM)
{
	m_pImpl = new Impl(pLuaVM);
}

SDE_State::~SDE_State()
{
	delete m_pImpl;
}

void SDE_GetGlobal(lua_State* pLuaVM)
{
	lua_pushstring(pLuaVM, SDE_GLOBAL);
	lua_rawget(pLuaVM, LUA_REGISTRYINDEX);
}

void SDE_GetSceneUserdata(lua_State* pLuaVM, SDE_Scene* pScene)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_SCENEUDATA);
	lua_rawget(pLuaVM, -2);
	lua_remove(pLuaVM, -2);

	lua_rawgeti(pLuaVM, -1, pScene->GetUserdataRef());
	lua_remove(pLuaVM, -2);
}

void SDE_GetSceneTable(lua_State* pLuaVM, SDE_Scene* pScene)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_SCENE);
	lua_rawget(pLuaVM, -2);
	lua_remove(pLuaVM, -2);

	lua_rawgeti(pLuaVM, -1, pScene->GetTableRef());
	lua_remove(pLuaVM, -2);
}

SDE_State* SDE_GetState(lua_State* pLuaVM)
{
	SDE_GetGlobal(pLuaVM);

	lua_pushstring(pLuaVM, SDE_GLOBAL_STATE);
	lua_rawget(pLuaVM, -2);

	SDE_State* pState = (SDE_State*)lua_touserdata(pLuaVM, -1);
	lua_pop(pLuaVM, 2);

	return pState;
}