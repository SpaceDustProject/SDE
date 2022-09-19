#ifndef _SDE_STATE_H_
#define _SDE_STATE_H_

#define SDE_GLOBAL				"SDE_GLOBAL"

#define SDE_GLOBAL_STATE		"SDE_GLOBAL_STATE"
#define SDE_GLOBAL_REGISTRY		"SDE_GLOBAL_REGISTRY"
#define SDE_GLOBAL_CONSTRUCTOR	"SDE_GLOBAL_CONSTRUCTOR"
#define SDE_GLOBAL_SCENE		"SDE_GLOBAL_SCENE"
#define SDE_GLOBAL_SCENEUDATA	"SDE_GLOBAL_SCENEUDATA"

#include "SDE_Constructor.h"
#include "SDE_Scene.h"

struct lua_State;

class SDE_State
{
public:
	SDE_Constructor* CreateConstructor(const char*, int);
	void DestroyConstructor(SDE_Constructor*);

	SDE_Scene* CreateScene();
	void DestroyScene(SDE_Scene*);

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_State(lua_State*);
	SDE_State(const SDE_State&) = delete;
	SDE_State& operator=(const SDE_State&) = delete;
	~SDE_State();
};

void SDE_GetGlobal(lua_State*);

void SDE_GetSceneUserdata(lua_State*, SDE_Scene*);

void SDE_GetSceneTable(lua_State*, SDE_Scene*);

SDE_State* SDE_GetState(lua_State*);

#endif // !_SDE_STATE_H_