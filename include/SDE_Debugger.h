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

	// �����Ϣ
	void Output(OutputType enumTypeOutput, const char* strFormat, ...);

	// ���� Debug �������
	void SetOuputFunc(SDE_OutputFunc funcOutputLog);

	// ���ü�¼����Ϣ�����С
	void SetBufferSize(size_t nSize);

	// �����Ƿ����ʱ��
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