#include "SDE_System.h"

#include "SDE_PackageTool.h"
#include "SDE_State.h"

#include <lua.hpp>

SDE_Scene* SDE_System::GetScene()
{
	return m_pScene;
}

const char* SDE_System::GetName()
{
	return m_strName;
}

void SDE_System::SetName(const char* strName)
{
	m_strName = strName;
}

SDE_SystemType SDE_System::GetType()
{
	return m_enumType;
}

void SDE_System::SetType(SDE_SystemType enumType)
{
	m_enumType = enumType;
}

bool SDE_System::IsRunning()
{
	return m_bIsRunning;
}

void SDE_System::SetRunningStatus(bool bIsRunning)
{
	m_bIsRunning = bIsRunning;
}

int SDE_System::GetFuncRef()
{
	return m_nRefFunc;
}

void SDE_System::SetFuncRef(int nRefFunc)
{
	m_nRefFunc = nRefFunc;
}

SDE_System* SDE_System::GetPrevSystem()
{
	return m_pSystemPrev;
}

void SDE_System::SetPrevSystem(SDE_System* pSystem)
{
	m_pSystemPrev = pSystem;
}

SDE_System* SDE_System::GetNextSystem()
{
	return m_pSystemNext;
}

void SDE_System::SetNextSystem(SDE_System* pSystem)
{
	m_pSystemNext = pSystem;
}

SDE_System::SDE_System(SDE_Scene* pScene)
	: SDE_Object(SDE_TYPE_SYSTEM)
{
	m_pScene = pScene;

	m_enumType = SDE_SystemType::NONE;
	m_strName = nullptr;
	m_bIsRunning = true;

	m_nRefFunc = LUA_NOREF;

	m_pSystemPrev = nullptr;
	m_pSystemNext = nullptr;
}