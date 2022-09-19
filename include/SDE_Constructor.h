#ifndef _SDE_CONSTRUCTOR_H_
#define _SDE_CONSTRUCTOR_H_

#define SDE_TYPE_CONSTRUCTOR "SDE_TYPE_CONSTRUCTOR"

struct lua_State;

class SDE_Constructor
{
public:
	const char* GetType() const;
	int			GetFuncRef() const;

private:
	const char*	m_strType;
	int			m_nRefFunc;

public:
	SDE_Constructor(const char* strType, int nRefFunc)
		: m_strType(strType), m_nRefFunc(nRefFunc) {}

	~SDE_Constructor() = default;
};

inline const char* SDE_Constructor::GetType() const
{
	return m_strType;
}

inline int SDE_Constructor::GetFuncRef() const
{
	return m_nRefFunc;
}

// Get the constructor on the specified index.
// This function will return nullptr when the object isn't a constructor.
SDE_Constructor* SDE_ToConstructor(lua_State*, int);

// Get the constructor on the specified index and check its metatable.
// This function will return nullptr when the object isn't a constructor or the metatable mismatch.
SDE_Constructor* SDE_CheckConstructor(lua_State*, int, const char* strMetatableName);

// Call constructor for a userdata on the specified index,
// if the table index is not zero, then it'll be passed in as a parameter.
void SDE_CallConstructor(lua_State* pState, SDE_Constructor*, int, int = 0);

#endif // !_SDE_CONSTRUCTOR_H_