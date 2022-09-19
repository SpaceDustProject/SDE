#ifndef _SDE_SYSTEM_H_
#define _SDE_SYSTEM_H_

#define SDE_TYPE_SYSTEM	"SDE_TYPE_SYSTEM"

#include "SDE_Object.h"

class SDE_Scene;

enum class SDE_SystemType
{
	NONE = 0,
	INITIALIZER,
	UPDATER,
	CONNECTOR
};

class SDE_System: public SDE_Object
{
public:
	SDE_Scene* GetScene();

	const char* GetName();
	void SetName(const char*);

	SDE_SystemType GetType();
	void SetType(SDE_SystemType);

	bool IsRunning();
	void SetRunningStatus(bool);

	int GetFuncRef();
	void SetFuncRef(int);

	SDE_System* GetPrevSystem();
	void SetPrevSystem(SDE_System*);

	SDE_System* GetNextSystem();
	void SetNextSystem(SDE_System*);

private:
	SDE_Scene*		m_pScene;

	SDE_SystemType	m_enumType;
	const char*		m_strName;
	bool			m_bIsRunning;

	int				m_nRefFunc;

	SDE_System*		m_pSystemPrev;
	SDE_System*		m_pSystemNext;

private:
	SDE_System(SDE_Scene*);
	~SDE_System() = default;

	friend class SDE_Scene;
};

#endif // !_SDE_SYSTEM_H_