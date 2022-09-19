#ifndef _SDE_ENTITY_H_
#define _SDE_ENTITY_H_

#define SDE_TYPE_ENTITY	"SDE_TYPE_ENTITY"

#include "SDE_Component.h"

class SDE_Scene;

class SDE_Entity : public SDE_Object
{
public:
	SDE_Scene* GetScene();

private:
	SDE_Scene* m_pScene;

private:
	SDE_Entity(SDE_Scene*);
	~SDE_Entity() = default;

	friend class SDE_Scene;
};

#endif // !_SDE_ENTITY_H_