#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#define SDE_TYPE_SCENE		"SDE_TYPE_SCENE"

#define SDE_SCENE_THREAD	"SDE_SCENE_THREAD"
#define SDE_SCENE_SYSTEM	"SDE_SCENE_SYSTEM"
#define SDE_SCENE_COMPONENT	"SDE_SCENE_COMPONENT"

#include "SDE_System.h"
#include "SDE_Entity.h"

#include <functional>

struct lua_State;

class SDE_Scene
{
public:
	void			Initialize();
	void			Update();

	int				GetTableRef();
	void			SetTableRef(int);

	int				GetUserdataRef();
	void			SetUserdataRef(int);

	SDE_System*		CreateSystem();
	void			DestroySystem(SDE_System*);
	SDE_System*		GetSystem(const char*);
	void			CallSystem(SDE_System*);

	SDE_Entity*		CreateEntity();
	void			DestroyEntity(SDE_Entity*);
	void			TraverseComponent(const char*, std::function<void(SDE_Component*)>);

	SDE_Component*	CreateComponent(SDE_Entity*);
	void			DestroyComponent(SDE_Component*);
	SDE_Component*	GetComponent(SDE_Entity*, const char*);
	void			TraverseComponent(SDE_Entity*, std::function<void(SDE_Component*)>);

	bool			RecordSystemInfo(SDE_System*);
	bool			RecordComponentInfo(SDE_Component*);

	void			AddInitializer(SDE_System*);
	void			DeleteInitializer(SDE_System*);

	void			AddUpdater(SDE_System*);
	void			DeleteUpdater(SDE_System*);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Scene(lua_State* pLuaVM);
	~SDE_Scene();

	friend class SDE_State;
};

#endif // !_SDE_SCENE_H_