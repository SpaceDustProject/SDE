#ifndef _SDE_COMPONENT_H_
#define _SDE_COMPONENT_H_

#define SDE_TYPE_COMPONENT	"SDE_TYPE_COMPONENT"

#include "SDE_Object.h"

class SDE_Entity;

class SDE_Component : public SDE_Object
{
public:
	SDE_Entity* GetEntity();

	const char* GetName();
	void SetName(const char*);

	int GetTableRef();
	void SetTableRef(int);

private:
	SDE_Entity* m_pEntity;
	const char* m_strName;
	int			m_nRefTable;

private:
	SDE_Component(SDE_Entity*);
	~SDE_Component() = default;

	friend class SDE_Scene;
};

#endif // !_SDE_COMPONENT_H_