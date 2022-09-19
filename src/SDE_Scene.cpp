#include "SDE_Scene.h"

#include "SDE_PackageTool.h"
#include "SDE_MemoryPool.h"
#include "SDE_State.h"
#include "SDE_Debugger.h"

#include <unordered_set>
#include <unordered_map>

#include <lua.hpp>

class SDE_Scene::Impl
{
public:
	lua_State*		m_pLuaVM;
	lua_State*		m_pThread;

	int				m_nRefTable;
	int				m_nRefUserdata;

	SDE_MemoryPool	m_memoryPool;

	std::unordered_set<
		SDE_System*
	> m_setInitializer;

	std::unordered_set<
		SDE_System*
	> m_setUpdater;

	std::unordered_map<
		const char*, SDE_System*
	> m_mapSystem;

	std::unordered_set<
		SDE_Entity*
	> m_setEntity;

	std::unordered_map<
		const char*, std::unordered_set<SDE_Component*>
	> m_mapComponent;

	std::unordered_map<
		SDE_Entity*, std::unordered_map<const char*, SDE_Component*>
	> m_mapEntityComponent;

public:
	Impl(lua_State* pLuaVM)
	{
		m_pLuaVM = pLuaVM;

		SDE_GetGlobal(pLuaVM);

		lua_pushstring(pLuaVM, SDE_GLOBAL_SCENE);
		lua_rawget(pLuaVM, -2);

		lua_createtable(pLuaVM, 0, 3);
		{
			lua_pushstring(pLuaVM, SDE_SCENE_THREAD);
			m_pThread = lua_newthread(pLuaVM);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_SCENE_SYSTEM);
			lua_newtable(pLuaVM);
			lua_rawset(pLuaVM, -3);

			lua_pushstring(pLuaVM, SDE_SCENE_COMPONENT);
			lua_newtable(pLuaVM);
			lua_rawset(pLuaVM, -3);
		}
		m_nRefTable = luaL_ref(pLuaVM, -2);
		m_nRefUserdata = LUA_NOREF;

		lua_pop(pLuaVM, 2);

		SDE_GetGlobal(m_pThread);

		lua_pushstring(m_pThread, SDE_GLOBAL_SCENE);
		lua_rawget(m_pThread, -2);
		lua_remove(m_pThread, -2);

		lua_rawgeti(m_pThread, -1, m_nRefTable);
		lua_remove(m_pThread, -2);

		lua_pushstring(m_pThread, SDE_SCENE_SYSTEM);
		lua_rawget(m_pThread, -2);
		lua_remove(m_pThread, -2);
	}
	~Impl()
	{
		lua_State* pLuaVM = m_pLuaVM;

		SDE_GetGlobal(pLuaVM);

		lua_pushstring(pLuaVM, SDE_GLOBAL_SCENE);
		lua_rawget(pLuaVM, -2);

		luaL_unref(pLuaVM, -1, m_nRefTable);

		lua_pop(pLuaVM, 2);
	}
};

void SDE_Scene::Initialize()
{
	for (auto& pSystem : m_pImpl->m_setInitializer)
	{
		for(SDE_System* pSystemIter = pSystem; pSystemIter; pSystemIter = pSystemIter->GetNextSystem())
		{
			CallSystem(pSystemIter);
		}
	}
}

void SDE_Scene::Update()
{
	for (auto& pSystem : m_pImpl->m_setUpdater)
	{
		for (SDE_System* pSystemIter = pSystem; pSystemIter; pSystemIter = pSystemIter->GetNextSystem())
		{
			CallSystem(pSystemIter);
		}
	}
}

int SDE_Scene::GetTableRef()
{
	return m_pImpl->m_nRefTable;
}

void SDE_Scene::SetTableRef(int nRefTable)
{
	m_pImpl->m_nRefTable = nRefTable;
}

int SDE_Scene::GetUserdataRef()
{
	return m_pImpl->m_nRefUserdata;
}

void SDE_Scene::SetUserdataRef(int nRefUserdata)
{
	m_pImpl->m_nRefUserdata = nRefUserdata;
}

SDE_System* SDE_Scene::CreateSystem()
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_System));
	SDE_System* pSystem = new (pMem) SDE_System(this);
	return pSystem;
}

void SDE_Scene::DestroySystem(SDE_System* pSystem)
{
	if (m_pImpl->m_mapSystem.find(pSystem->GetName()) == m_pImpl->m_mapSystem.end())
		return;

	m_pImpl->m_mapSystem.erase(pSystem->GetName());

	pSystem->~SDE_System();
	m_pImpl->m_memoryPool.Free(pSystem, sizeof(SDE_System));
}

SDE_System* SDE_Scene::GetSystem(const char* strName)
{
	if (m_pImpl->m_mapSystem.find(strName) == m_pImpl->m_mapSystem.end())
		return nullptr;

	return m_pImpl->m_mapSystem[strName];
}

void SDE_Scene::CallSystem(SDE_System* pSystem)
{
	if (pSystem->GetScene() != this)
		return;

	if (pSystem->GetFuncRef() != LUA_NOREF && pSystem->IsRunning())
	{
		lua_rawgeti(m_pImpl->m_pThread, 1, pSystem->GetFuncRef());
		lua_pushlightuserdata(m_pImpl->m_pThread, pSystem);

		if (lua_pcall(m_pImpl->m_pThread, 1, 0, 0))
		{
			SDE_Debugger::Instance().Output(
				SDE_Debugger::OutputType::ERROR,
				"Failed to call system's function, %s.",
				lua_tostring(m_pImpl->m_pThread, -1)
			);
			lua_pop(m_pImpl->m_pThread, 1);
		}
	}
}

SDE_Entity* SDE_Scene::CreateEntity()
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_Entity));
	SDE_Entity* pEntity = new (pMem) SDE_Entity(this);
	m_pImpl->m_setEntity.insert(pEntity);
	return pEntity;
}

void SDE_Scene::DestroyEntity(SDE_Entity* pEntity)
{
	if (m_pImpl->m_setEntity.find(pEntity) == m_pImpl->m_setEntity.end())
		return;

	m_pImpl->m_setEntity.erase(pEntity);

	pEntity->~SDE_Entity();
	m_pImpl->m_memoryPool.Free(pEntity, sizeof(SDE_Entity));
}

SDE_Component* SDE_Scene::CreateComponent(SDE_Entity* pEntity)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_Component));
	SDE_Component* pComponent = new (pMem) SDE_Component(pEntity);
	return pComponent;
}

SDE_Component* SDE_Scene::GetComponent(SDE_Entity* pEntity, const char* strType)
{
	if (m_pImpl->m_mapEntityComponent.find(pEntity) == m_pImpl->m_mapEntityComponent.end())
		return nullptr;

	if (m_pImpl->m_mapEntityComponent[pEntity].find(strType) == m_pImpl->m_mapEntityComponent[pEntity].end())
		return nullptr;

	return m_pImpl->m_mapEntityComponent[pEntity][strType];
}

void SDE_Scene::TraverseComponent(SDE_Entity* pEntity, std::function<void(SDE_Component*)> funcCalled)
{
	if (m_pImpl->m_setEntity.find(pEntity) == m_pImpl->m_setEntity.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to traverse entity's component, because the entity doesn't belong to the scene."
		);
		return;
	}
	
	for (std::unordered_map<const char*, SDE_Component*>::iterator iter = m_pImpl->m_mapEntityComponent[pEntity].begin();
		iter != m_pImpl->m_mapEntityComponent[pEntity].end(); iter++)
	{
		funcCalled((*iter).second);
	}
}

void SDE_Scene::DestroyComponent(SDE_Component* pComponent)
{
	m_pImpl->m_mapComponent[pComponent->GetType()].erase(pComponent);
	m_pImpl->m_mapEntityComponent[pComponent->GetEntity()].erase(pComponent->GetType());

	pComponent->~SDE_Component();
	m_pImpl->m_memoryPool.Free(pComponent, sizeof(SDE_Component));
}

bool SDE_Scene::RecordSystemInfo(SDE_System* pSystem)
{
	if (!pSystem->GetName())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create the system, because you didn't name the system."
		);
		return false;
	}
	if (pSystem->GetFuncRef() == LUA_NOREF)
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create a system of type %s, because you haven't set a function for system %s!",
			pSystem->GetName()
		);
		return false;
	}
	if (m_pImpl->m_mapSystem.find(pSystem->GetName()) != m_pImpl->m_mapSystem.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create a system of type %s, because a system named %s already exists in the scene.",
			pSystem->GetName()
		);
		return false;
	}
	m_pImpl->m_mapSystem[pSystem->GetName()] = pSystem;

	return true;
}

bool SDE_Scene::RecordComponentInfo(SDE_Component* pComponent)
{
	if (m_pImpl->m_setEntity.find(pComponent->GetEntity()) == m_pImpl->m_setEntity.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create a component of name %s, because the entity doesn't exist.",
			pComponent->GetName()
		);
		return false;
	}
	if (!pComponent->GetName())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create a component, because you haven't set a name for component!"
		);
		return false;
	}
	if (m_pImpl->m_mapEntityComponent[pComponent->GetEntity()].find(pComponent->GetName()) != m_pImpl->m_mapEntityComponent[pComponent->GetEntity()].end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to create a component of name %s, because this entity already has components of this name.",
			pComponent->GetName()
		);
		return false;
	}

	m_pImpl->m_mapComponent[pComponent->GetName()].insert(pComponent);
	m_pImpl->m_mapEntityComponent[pComponent->GetEntity()][pComponent->GetName()] = pComponent;

	return true;
}

void SDE_Scene::TraverseComponent(const char* strType, std::function<void(SDE_Component*)> funcCalled)
{
	if (m_pImpl->m_mapComponent.find(strType) == m_pImpl->m_mapComponent.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Failed to traverse scene's component of type %s, because this type of component does not exist.",
			strType
		);
		return;
	}

	for (std::unordered_set<SDE_Component*>::iterator iter = m_pImpl->m_mapComponent[strType].begin();
		iter != m_pImpl->m_mapComponent[strType].end(); iter++)
	{
		funcCalled(*iter);
	}
}

void SDE_Scene::AddInitializer(SDE_System* pSystem)
{
	if (m_pImpl->m_setInitializer.find(pSystem) != m_pImpl->m_setInitializer.end())
		return;

	m_pImpl->m_setInitializer.insert(pSystem);
}

void SDE_Scene::DeleteInitializer(SDE_System* pSystem)
{
	if (m_pImpl->m_setInitializer.find(pSystem) != m_pImpl->m_setInitializer.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Scene %s cannot find such a system to delete."
		);
	}
	m_pImpl->m_setInitializer.erase(pSystem);
}

void SDE_Scene::AddUpdater(SDE_System* pSystem)
{
	if (m_pImpl->m_setInitializer.find(pSystem) != m_pImpl->m_setInitializer.end())
		return;

	m_pImpl->m_setUpdater.insert(pSystem);
}

void SDE_Scene::DeleteUpdater(SDE_System* pSystem)
{
	if (m_pImpl->m_setUpdater.find(pSystem) != m_pImpl->m_setUpdater.end())
	{
		SDE_Debugger::Instance().Output(
			SDE_Debugger::OutputType::WARNING,
			"Scene %s cannot find such a system to delete."
		);
	}
	m_pImpl->m_setUpdater.erase(pSystem);
}

SDE_Scene::SDE_Scene(lua_State* pLuaVM)
{
	m_pImpl = new Impl(pLuaVM);
}

SDE_Scene::~SDE_Scene()
{
	delete m_pImpl;
}