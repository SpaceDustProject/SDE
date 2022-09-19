#ifndef _SDE_OBJECT_H_
#define _SDE_OBJECT_H_

struct lua_State;

enum class SDE_ObjectType
{
	SCENE = 0,
	SYSTEM,
	ENTITY,
	COMPONENT
};

class SDE_Object
{
public:
	const char* GetType();

	bool		IsConstructed();
	void		SetCompleted();

private:
	const char* m_strType;
	bool		m_bIsConstructed;

public:
	SDE_Object(const char* strType);
	virtual ~SDE_Object() = default;
};

inline SDE_Object::SDE_Object(const char* strType)
{
	m_strType = strType;
	m_bIsConstructed = false;
}

inline const char* SDE_Object::GetType()
{
	return m_strType;
}

inline bool SDE_Object::IsConstructed()
{
	return m_bIsConstructed;
}

inline void SDE_Object::SetCompleted()
{
	m_bIsConstructed = true;
}

SDE_Object* SDE_CheckObject(lua_State*, int, const char*);

#endif // !_SDE_OBJECT_H_