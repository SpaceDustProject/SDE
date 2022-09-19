#ifndef _SDE_DEBUGGER_H_
#define _SDE_DEBUGGER_H_

typedef void(*SDE_OutputFunc)(const char*);

class SDE_Debugger
{
public:
	enum class OutputType
	{
		INFO = 0,
		WARNING,
		ERROR
	};

	// 输出信息
	void Output(OutputType enumTypeOutput, const char* strFormat, ...);

	// 设置 Debug 输出函数
	void SetOuputFunc(SDE_OutputFunc funcOutputLog);

	// 设置记录器信息缓存大小
	void SetBufferSize(size_t nSize);

	// 设置是否输出时间
	void SetOutputTime(bool bCanOutput);

private:
	SDE_OutputFunc	m_funcOutput;
	size_t			m_nBufferSize;
	char*			m_strBuffer;
	bool			m_bCanOutputTime;

public:
	~SDE_Debugger() = default;
	SDE_Debugger(const SDE_Debugger&) = delete;
	SDE_Debugger& operator=(const SDE_Debugger&) = delete;
	static SDE_Debugger& Instance() {
		static SDE_Debugger instance;
		return instance;
	}
private:
	SDE_Debugger();
};

#endif // !_SDE_DEBUGGER_H_