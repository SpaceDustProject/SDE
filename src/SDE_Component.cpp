#include "SDE_Component.h"

#include "SDE_PackageTool.h"

#include <lua.hpp>

SDE_Entity* SDE_Component::GetEntity()
{
	return m_pEntity;
}

const char* SDE_Component::GetName()
{
	return m_strName;
}

void SDE_Component::SetName(const char* strName)
{
	m_strName = strName;
}

int SDE_Component::GetTableRef()
{
	return m_nRefTable;
}

void SDE_Component::SetTableRef(int nRefTable)
{
	m_nRefTable = nRefTable;
}

SDE_Component::SDE_Component(SDE_Entity* pEntity)
	: SDE_Object(SDE_TYPE_COMPONENT)
{
	m_pEntity = pEntity;
	m_strName = nullptr;
	m_nRefTable = LUA_NOREF;
}