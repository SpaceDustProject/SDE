#include "SDE_PackageTool.h"

#include <lua.hpp>
#include <new>
#include <list>

class SDE_PackageTool::LuaEnumRegList::Impl
{
public:
	std::list<LuaEnumReg> m_listLuaEnumReg;

public:
	Impl(const std::initializer_list<LuaEnumReg>& ilLuaEnumReg) :
		m_listLuaEnumReg(ilLuaEnumReg) {}
};

size_t SDE_PackageTool::LuaEnumRegList::GetSize() const
{
	return m_pImpl->m_listLuaEnumReg.size();
}

void SDE_PackageTool::LuaEnumRegList::Traverse(std::function<void(const SDE_PackageTool::LuaEnumReg&)> funcCalled) const
{
	for (std::list<LuaEnumReg>::iterator iter = m_pImpl->m_listLuaEnumReg.begin();
		iter != m_pImpl->m_listLuaEnumReg.end(); iter++)
	{
		funcCalled(*iter);
	}
}

SDE_PackageTool::LuaEnumRegList& SDE_PackageTool::LuaEnumRegList::operator=(const LuaEnumRegList& listEnumReg)
{
	if (m_pImpl) delete m_pImpl;
	m_pImpl = new Impl(*listEnumReg.m_pImpl);
	return *this;
}

SDE_PackageTool::LuaEnumRegList::LuaEnumRegList(const std::initializer_list<LuaEnumReg>& ilLuaEnumReg)
{
	m_pImpl = new Impl(ilLuaEnumReg);
}

SDE_PackageTool::LuaEnumRegList::LuaEnumRegList(const LuaEnumRegList& listEnumReg)
{
	m_pImpl = new Impl(*listEnumReg.m_pImpl);
}

SDE_PackageTool::LuaEnumRegList::~LuaEnumRegList()
{
	delete m_pImpl;
}

class SDE_PackageTool::LuaFuncRegList::Impl
{
public:
	std::list<LuaFuncReg> m_listLuaFuncReg;

public:
	Impl(const std::initializer_list<LuaFuncReg>& ilLuaFuncReg) :
		m_listLuaFuncReg(ilLuaFuncReg) {}
};

size_t SDE_PackageTool::LuaFuncRegList::GetSize() const
{
	return m_pImpl->m_listLuaFuncReg.size();
}

void SDE_PackageTool::LuaFuncRegList::Traverse(std::function<void(const SDE_PackageTool::LuaFuncReg&)> funcCalled) const
{
	for (std::list<LuaFuncReg>::iterator iter = m_pImpl->m_listLuaFuncReg.begin();
		iter != m_pImpl->m_listLuaFuncReg.end(); iter++)
	{
		funcCalled(*iter);
	}
}

SDE_PackageTool::LuaFuncRegList& SDE_PackageTool::LuaFuncRegList::operator=(const LuaFuncRegList& listFuncReg)
{
	if (m_pImpl) delete m_pImpl;
	m_pImpl = new Impl(*listFuncReg.m_pImpl);
	return *this;
}

SDE_PackageTool::LuaFuncRegList::LuaFuncRegList(const std::initializer_list<LuaFuncReg>& ilLuaFuncReg)
{
	m_pImpl = new Impl(ilLuaFuncReg);
}

SDE_PackageTool::LuaFuncRegList::LuaFuncRegList(const LuaFuncRegList& listFuncReg)
{
	m_pImpl = new Impl(*listFuncReg.m_pImpl);
}

SDE_PackageTool::LuaFuncRegList::~LuaFuncRegList()
{
	delete m_pImpl;
}

class SDE_PackageTool::LuaMetatableRegList::Impl
{
public:
	std::list<LuaMetatableReg> m_listMetatableReg;

public:
	Impl(const std::initializer_list<LuaMetatableReg>& ilMetatableReg) :
		m_listMetatableReg(ilMetatableReg) {}
};

size_t SDE_PackageTool::LuaMetatableRegList::GetSize() const
{
	return m_pImpl->m_listMetatableReg.size();
}

void SDE_PackageTool::LuaMetatableRegList::Traverse(std::function<void(const LuaMetatableReg&)> funcCalled) const
{
	for (std::list<LuaMetatableReg>::iterator iter = m_pImpl->m_listMetatableReg.begin();
		iter != m_pImpl->m_listMetatableReg.end(); iter++)
	{
		funcCalled(*iter);
	}
}

SDE_PackageTool::LuaMetatableRegList& SDE_PackageTool::LuaMetatableRegList::operator=(const LuaMetatableRegList& listMetatableReg)
{
	if (m_pImpl) delete m_pImpl;
	m_pImpl = new Impl(*listMetatableReg.m_pImpl);
	return *this;
}

SDE_PackageTool::LuaMetatableRegList::LuaMetatableRegList(const std::initializer_list<LuaMetatableReg>& ilMetatableReg)
{
	m_pImpl = new Impl(ilMetatableReg);
}

SDE_PackageTool::LuaMetatableRegList::LuaMetatableRegList(const LuaMetatableRegList& listMetatableReg)
{
	m_pImpl = new Impl(*listMetatableReg.m_pImpl);
}

SDE_PackageTool::LuaMetatableRegList::~LuaMetatableRegList()
{
	delete m_pImpl;
}

class SDE_PackageTool::LuaPackage::Impl
{
public:
	std::string				m_strName;
	LuaFuncRegList			m_listFuncReg;
	LuaEnumRegList			m_listEnumReg;
	LuaMetatableRegList		m_listMetatableReg;
	LuaFuncV				m_funcInit;
	LuaFunc					m_funcQuit;

public:
	Impl(
		const std::string& strName,
		const std::initializer_list<LuaFuncReg>& ilLuaFuncReg,
		const std::initializer_list<LuaEnumReg>& ilLuaEnumReg,
		const std::initializer_list<LuaMetatableReg>& ilLuaMetatableReg,
		LuaFuncV funcInit,
		LuaFunc funcQuit
	) :
		m_strName(strName),
		m_listFuncReg(ilLuaFuncReg),
		m_listEnumReg(ilLuaEnumReg),
		m_listMetatableReg(ilLuaMetatableReg),
		m_funcInit(funcInit),
		m_funcQuit(funcQuit)
	{}
};

const std::string& SDE_PackageTool::LuaPackage::GetName() const
{
	return m_pImpl->m_strName;
}

size_t SDE_PackageTool::LuaPackage::GetSize() const
{
	return m_pImpl->m_listFuncReg.GetSize() + m_pImpl->m_listEnumReg.GetSize();
}

const SDE_PackageTool::LuaFuncRegList& SDE_PackageTool::LuaPackage::GetFuncRegList() const
{
	return m_pImpl->m_listFuncReg;
}

const SDE_PackageTool::LuaEnumRegList& SDE_PackageTool::LuaPackage::GetEnumRegList() const
{
	return m_pImpl->m_listEnumReg;
}

const SDE_PackageTool::LuaMetatableRegList& SDE_PackageTool::LuaPackage::GetMetatableRegList() const
{
	return m_pImpl->m_listMetatableReg;
}

SDE_PackageTool::LuaFuncV SDE_PackageTool::LuaPackage::GetInitFunc() const
{
	return m_pImpl->m_funcInit;
}

SDE_PackageTool::LuaFunc SDE_PackageTool::LuaPackage::GetQuitFunc() const
{
	return m_pImpl->m_funcQuit;
}

SDE_PackageTool::LuaPackage& SDE_PackageTool::LuaPackage::operator=(const LuaPackage& package)
{
	if (m_pImpl) delete m_pImpl;
	m_pImpl = new Impl(*package.m_pImpl);
	return *this;
}

SDE_PackageTool::LuaPackage::LuaPackage(
	const std::string& strName,
	const std::initializer_list<LuaFuncReg>& ilLuaFuncReg,
	const std::initializer_list<LuaEnumReg>& ilLuaEnumReg,
	const std::initializer_list<LuaMetatableReg>& ilLuaMetatableReg,
	LuaFuncV funcInit,
	LuaFunc funcQuit
)
{
	m_pImpl = new Impl(strName, ilLuaFuncReg, ilLuaEnumReg, ilLuaMetatableReg, funcInit, funcQuit);
}

SDE_PackageTool::LuaPackage::LuaPackage(const LuaPackage& package)
{
	m_pImpl = new Impl(*package.m_pImpl);
}

SDE_PackageTool::LuaPackage::~LuaPackage()
{
	delete m_pImpl;
}

void SDE_PackageTool::SetLuaEnumList(lua_State* pState, const LuaEnumRegList& listEnumReg)
{
	if (lua_type(pState, -1) != LUA_TTABLE)
	{
		luaL_error(pState, "Error occered when try to set a lua's enum list: The object on the top of the stack isn't table.");
	}

	listEnumReg.Traverse(
		[&pState](const LuaEnumReg& regEnum)
		{
			lua_pushstring(pState, regEnum.name);
			lua_pushinteger(pState, regEnum.value);
			lua_rawset(pState, -3);
		}
	);
}

void SDE_PackageTool::SetLuaFuncList(lua_State* pState, const LuaFuncRegList& listFuncReg)
{
	if (lua_type(pState, -1) != LUA_TTABLE)
	{
		luaL_error(pState, "Error occered when try to set a lua's function list: The object on the top of the stack isn't table.");
	}

	listFuncReg.Traverse(
		[&pState](const LuaFuncReg& regEnum)
		{
			lua_pushstring(pState, regEnum.name);
			lua_pushcfunction(pState, regEnum.func);
			lua_rawset(pState, -3);
		}
	);
}

void SDE_PackageTool::RegisterLuaMetatable(lua_State* pState, const LuaMetatableReg& regMetatable)
{
	luaL_newmetatable(pState, regMetatable.GetName().c_str());

	lua_pushstring(pState, "__index");
	lua_createtable(pState, 0, (int)regMetatable.GetFuncRegList().GetSize());
	SetLuaFuncList(pState, regMetatable.GetFuncRegList());
	lua_rawset(pState, -3);

	if (regMetatable.GetGCFunc())
	{
		lua_pushstring(pState, "__gc");
		lua_pushcfunction(pState, regMetatable.GetGCFunc());
		lua_rawset(pState, -3);
	}

	lua_pop(pState, 1);
}

void SDE_PackageTool::SetLuaPackage(lua_State* pState, const LuaPackage& package)
{
	if (lua_type(pState, -1) != LUA_TTABLE)
	{
		luaL_error(pState, "Error occered when try to set a lua's package: The object on the top of the stack isn't table.");
	}

	// Call the package's initialization function.
	if (package.GetInitFunc())
	{
		package.GetInitFunc()(pState);
	}

	// Set all functions and enumeration to the package on the top of the stack.
	SetLuaFuncList(pState, package.GetFuncRegList());
	SetLuaEnumList(pState, package.GetEnumRegList());

	// Register all metatable into the registry.
	package.GetMetatableRegList().Traverse(
		[&pState](const SDE_PackageTool::LuaMetatableReg& regMetatable)
		{
			RegisterLuaMetatable(pState, regMetatable);
		}
	);

	// Set a metatable for the table on the top of the stack.
	lua_newtable(pState);
	{
		lua_pushstring(pState, "__name");
		lua_pushstring(pState, package.GetName().c_str());
		lua_rawset(pState, -3);

		if (package.GetQuitFunc())
		{
			lua_pushstring(pState, "__gc");
			lua_pushcfunction(pState, package.GetQuitFunc());
			lua_rawset(pState, -3);
		}
	}
	lua_setmetatable(pState, -2);
}

void SDE_PackageTool::SetLuaPreloader(lua_State* pState, const std::string& strPackageName, LuaFunc funcCalled)
{
	lua_getglobal(pState, "package");

	lua_pushstring(pState, "preload");
	lua_rawget(pState, -2);

	lua_pushstring(pState, strPackageName.c_str());
	lua_pushcfunction(pState, funcCalled);
	lua_rawset(pState, -3);

	lua_pop(pState, 2);
}

void SDE_PackageTool::TraverseStack(lua_State* pState, std::function<bool()> funcCalled)
{
	bool bContinue = true;
	for (size_t i = 0; i < lua_gettop(pState) && bContinue; i++)
	{
		lua_pushvalue(pState, 1);
		bContinue = funcCalled();
		lua_pop(pState, 1);
	}
}

void SDE_PackageTool::TraverseTable(lua_State* pState, int nIndex, std::function<bool()> funcCalled)
{
	if (!lua_istable(pState, -1))
	{
		luaL_error(pState, "Error occered when traverse a lua's table: The object on the top of the stack isn't table.");
	}
	
	bool bContinue = true;
	lua_pushnil(pState);
	while (bContinue && lua_next(pState, nIndex))
	{
		bContinue = funcCalled();

		if (bContinue)
		{
			lua_pop(pState, 1);
		}
		else lua_pop(pState, 2);
	}
}

void* SDE_PackageTool::NewUserdata(lua_State* pState, size_t nSize, const std::string& strMetatableName)
{
	void* pUserdata = lua_newuserdata(pState, nSize);
	luaL_setmetatable(pState, strMetatableName.c_str());
	return pUserdata;
}

void* SDE_PackageTool::ToUserdata(lua_State* pState, int nIndex, const std::string& strMetatableName)
{
	if (!lua_isuserdata(pState, nIndex))
		return nullptr;

	void* pUserdata = lua_touserdata(pState, nIndex);

	if (CheckMetatable(pState, nIndex, strMetatableName))
		return pUserdata;

	return nullptr;
}

void SDE_PackageTool::NewAccessor(lua_State* pState, void* pUserdata, const std::string& strMetatableName)
{
	void** ppAccessor = (void**)NewUserdata(pState, sizeof(void*), strMetatableName);
	*ppAccessor = pUserdata;
}

void* SDE_PackageTool::ToAccessor(lua_State* pState, int nIndex, const std::string& strMetatableName)
{
	if (!lua_isuserdata(pState, nIndex))
		return nullptr;

	void** ppAccessor = (void**)lua_touserdata(pState, nIndex);

	if (CheckMetatable(pState, nIndex, strMetatableName))
		return *ppAccessor;

	return nullptr;
}

bool SDE_PackageTool::CheckMetatable(lua_State* pState, int nIndex, const std::string& strMetatableName)
{
	if (lua_getmetatable(pState, nIndex))
	{
		lua_pushstring(pState, "__name");
		lua_rawget(pState, -2);

		std::string strName = lua_tostring(pState, -1);
		lua_pop(pState, 2);

		if (strName == strMetatableName)
			return true;
	}
	return false;
}