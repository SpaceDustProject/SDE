#include "SDE_Debugger.h"

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <functional>

// ��ʱ��Ϊ��λ��ʱǰ׺���� "0" ��Ϊʱ�ӷ��
std::string SDE_Debugger_ToClock(int num)
{
	std::string strOutput = std::to_string(num);
	if (num < 10) strOutput = "0" + strOutput;
	return strOutput;
}

// �ڵ�����Ϣǰ����ʱ��
std::string SDE_Debugger_AppendTime(const std::string& strInput)
{
	std::string strOutput = strInput;

	time_t timeNow = time(0);
	tm timeLocal;
	localtime_s(&timeLocal, &timeNow);

	strOutput =
		"[" + SDE_Debugger_ToClock(timeLocal.tm_hour) +
		":" + SDE_Debugger_ToClock(timeLocal.tm_min) +
		":" + SDE_Debugger_ToClock(timeLocal.tm_sec) +
		"] " + strInput;

	return strOutput;
}

std::string SDE_Debugger_CastOutputType(SDE_Debugger::OutputType enumTypeOutput)
{
	switch (enumTypeOutput)
	{
	case SDE_Debugger::OutputType::INFO:
		return "[INFO]";

	case SDE_Debugger::OutputType::WARNING:
		return "[WARNING]";

	case SDE_Debugger::OutputType::ERROR:
		return "[ERROR]";

	default:
		return "[UNKNOWN]";
	}
}

void SDE_Debugger::Output(SDE_Debugger::OutputType enumTypeOutput, const char* strFormat, ...)
{
	va_list listArgs;
	va_start(listArgs, strFormat);
	vsnprintf(m_strBuffer, m_nBufferSize, strFormat, listArgs);
	va_end(listArgs);

	std::string strPrefix = SDE_Debugger_CastOutputType(enumTypeOutput);
	std::string strOutput = m_strBuffer;
	strOutput = strPrefix + ": " + strOutput + "\n";

	if (m_bCanOutputTime)
	{
		strOutput = SDE_Debugger_AppendTime(strOutput);
	}
	strcpy_s(m_strBuffer, m_nBufferSize, strOutput.c_str());

	m_funcOutput(m_strBuffer);

	if (enumTypeOutput == SDE_Debugger::OutputType::ERROR) abort();
}

void SDE_Debugger::SetOuputFunc(SDE_OutputFunc funcOutputLog)
{
	m_funcOutput = funcOutputLog;
}

void SDE_Debugger::SetBufferSize(size_t nSize)
{
	// ɾ��ԭ���Ļ�����
	delete[] m_strBuffer;

	// �����µĻ�����
	m_nBufferSize = nSize;
	m_strBuffer = new char[nSize];
}

void SDE_Debugger::SetOutputTime(bool bCanOutputTime)
{
	m_bCanOutputTime = bCanOutputTime;
}

SDE_Debugger::SDE_Debugger()
{
	// Ĭ�ϻ�������С 4096 �ֽ�
	m_nBufferSize = 4096;

	m_strBuffer = new char[m_nBufferSize];

	// Ĭ����־�������(���������̨)
	m_funcOutput = [](const char* strContent)->void
	{
		printf_s(strContent);
	};

	// Ĭ�����ʱ��
	m_bCanOutputTime = true;
}