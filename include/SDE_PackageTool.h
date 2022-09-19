#ifndef _SDE_PACKAGE_TOOL_H_
#define _SDE_PACKAGE_TOOL_H_

#define SDE_API extern "C"

#define SDE_LUA_FUNC_X(FUNC_NAME, STATE_NAME) \
		SDE_API int FUNC_NAME(lua_State* STATE_NAME)

#define SDE_LUA_FUNC(FUNC_NAME) SDE_LUA_FUNC_X(FUNC_NAME, pLuaVM)

#ifdef _MSC_VER
#define SDE_PACKAGE_MAIN(PACKAGE_NAME)		\
		SDE_API __declspec(dllexport) int	\
		luaopen_ ## PACKAGE_NAME(lua_State* pLuaVM)
#else
#define SDE_PACKAGE_MAIN(PACKAGE_NAME)	\
		SDE_API int						\
		luaopen_ ## PACKAGE_NAME(lua_State* pLuaVM)
#endif // _MSC_VER

#include <string>
#include <functional>
#include <initializer_list>

struct lua_State;

namespace SDE_PackageTool
{
	typedef int (*LuaFunc)(lua_State*);
	typedef void (*LuaFuncV)(lua_State*);

	struct LuaFuncReg
	{
		const char* name;
		LuaFunc func;
	};

	struct LuaEnumReg
	{
		const char* name;
		long long value;
	};

	class LuaEnumRegList
	{
	public:
		// Get the size of the enum list.
		size_t GetSize() const;

		// Traverse all enum in the list and call the function for them.
		void Traverse(std::function<void(const LuaEnumReg&)> funcCalled) const;

		LuaEnumRegList& operator=(const LuaEnumRegList& listEnumReg);

	private:
		class Impl;
		Impl* m_pImpl;

	public:
		LuaEnumRegList(const std::initializer_list<LuaEnumReg>& ilLuaEnumReg);
		LuaEnumRegList(const LuaEnumRegList& listEnumReg);
		~LuaEnumRegList();
	};

	class LuaFuncRegList
	{
	public:
		// Get the size of the func list.
		size_t GetSize() const;

		// Traverse all func in the list and call the function for them.
		void Traverse(std::function<void(const LuaFuncReg&)> funcCalled) const;

		LuaFuncRegList& operator=(const LuaFuncRegList& listFuncReg);

	private:
		class Impl;
		Impl* m_pImpl;

	public:
		LuaFuncRegList(const std::initializer_list<LuaFuncReg>& ilLuaFuncReg);
		LuaFuncRegList(const LuaFuncRegList& listFuncReg);
		~LuaFuncRegList();
	};

	class LuaMetatableReg
	{
	public:
		const std::string& GetName() const {
			return m_strName;
		}

		const LuaFuncRegList& GetFuncRegList() const {
			return m_listFuncReg;
		}

		LuaFunc	GetGCFunc() const {
			return m_funcGC;
		}

	private:
		std::string		m_strName;
		LuaFuncRegList	m_listFuncReg;
		LuaFunc			m_funcGC;

	public:
		LuaMetatableReg(
			const std::string& strName,
			const std::initializer_list<LuaFuncReg>& ilLuaFuncReg,
			LuaFunc funcGC = nullptr
		) :
			m_strName(strName), m_listFuncReg(ilLuaFuncReg), m_funcGC(funcGC) {}
	};

	class LuaMetatableRegList
	{
	public:
		// Get the size of the metatable list.
		size_t GetSize() const;

		// Traverse all metatable in the list and call the function for them.
		void Traverse(std::function<void(const LuaMetatableReg&)> funcCalled) const;

		LuaMetatableRegList& operator=(const LuaMetatableRegList& listMetatableReg);

	private:
		class Impl;
		Impl* m_pImpl;

	public:
		LuaMetatableRegList(const std::initializer_list<LuaMetatableReg>& ilMetatableReg);
		LuaMetatableRegList(const LuaMetatableRegList& listMetatableReg);
		~LuaMetatableRegList();
	};

	class LuaPackage
	{
	public:
		// Get the name of the package.
		const std::string& GetName() const;

		// Get the sum of function list and enum list sizes.
		size_t GetSize() const;

		// Get the func list of the package.
		const LuaFuncRegList& GetFuncRegList() const;

		// Get the enum list of the package.
		const LuaEnumRegList& GetEnumRegList() const;

		// Get the metatable list of the package.
		const LuaMetatableRegList& GetMetatableRegList() const;

		// Get the init function of the package.
		LuaFuncV GetInitFunc() const;

		// Get the quit function of the package.
		LuaFunc GetQuitFunc() const;

		LuaPackage& operator=(const LuaPackage& package);

	private:
		class Impl;
		Impl* m_pImpl;

	public:
		LuaPackage(
			const std::string& strName,
			const std::initializer_list<LuaFuncReg>& ilLuaFuncReg,
			const std::initializer_list<LuaEnumReg>& ilLuaEnumReg,
			const std::initializer_list<LuaMetatableReg>& ilLuaMetatableReg,
			LuaFuncV funcInit = nullptr,
			LuaFunc funcQuit = nullptr
		);
		LuaPackage(const LuaPackage& package);

		~LuaPackage();
	};

	// Set all enum in the list into the table on the top of the stack.
	void SetLuaEnumList(lua_State* pState, const LuaEnumRegList& listEnumReg);

	// Set all functions in the list into the table on the top of the stack.
	void SetLuaFuncList(lua_State* pState, const LuaFuncRegList& listFuncReg);

	// Register the metatable into the registry of the state.
	void RegisterLuaMetatable(lua_State* pState, const LuaMetatableReg& regMetatable);

	// Register the package's metatable and set the package's data into the table on the top of the stack.
	// You'd better set only one package for a table, because this function will set a metatable for the table.
	void SetLuaPackage(lua_State* pState, const LuaPackage& package);

	// Set a loader for preload package.
	void SetLuaPreloader(lua_State* pState, const std::string& strPackageName, LuaFunc funcCalled);

	// Traverse the stack of virtual machine and call the function on each object.
	void TraverseStack(lua_State* pState, std::function<bool()> funcCalled);

	// Traverse a table and call the function on each object.
	void TraverseTable(lua_State* pState, int nIndex, std::function<bool()> funcCalled);

	// Create a userdata on the top of the stack and set a metatable for it.
	void* NewUserdata(lua_State* pState, size_t nSize, const std::string& strMetatableName);

	// Get a userdata at the specified index and check it's metatable.
	// Unlike luaL_checkudata, this function wouldn't cause an abort when the metatable doesn't match.
	void* ToUserdata(lua_State* pState, int nIndex, const std::string& strMetatableName);

	// Create a accessor on the top of the stack and set a metatable for it.
	void NewAccessor(lua_State* pState, void* pUserdata, const std::string& strMetatableName);

	// Get userdata on the specified index and check its metatable.
	void* ToAccessor(lua_State* pState, int nIndex, const std::string& strMetatableName);

	// Check the metatable of the object on the specified index.
	bool CheckMetatable(lua_State* pState, int nIndex, const std::string& strMetatableName);
}

#endif // !_SDE_PACKAGE_TOOL_H_