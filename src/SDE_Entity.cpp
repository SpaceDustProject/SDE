#include "SDE_Entity.h"

#include "SDE_PackageTool.h"

#include <lua.hpp>

SDE_Scene* SDE_Entity::GetScene()
{
	return m_pScene;
}

SDE_Entity::SDE_Entity(SDE_Scene* pScene)
	: SDE_Object(SDE_TYPE_ENTITY)
{
	m_pScene = pScene;
}