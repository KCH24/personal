#pragma once
#ifndef __HTTP_MESSAGE_ANALYZER__
#define __HTTP_MESSAGE_ANALYZER__

#include "ErrorCodeList.h"
#include "GlobalDefine.h"


class CHttpMessageAnalyzer
{
public:
	CHttpMessageAnalyzer();

	CHttpMessageAnalyzer(char* cReceivedMessage, unsigned int intMessageSize);

	virtual ~CHttpMessageAnalyzer();

	unsigned int GetRequestType() { return m_intRequestType; };
	unsigned int GetOpsType() { return m_intOpsType; };
	char* GetRequestOpsData() { return m_cRequest; };
	unsigned int GetReturnCode() { return m_intReturnCode; };
	bool IsHex(char cValue);

private:
	char* cMessage;

	char* m_cRequest;
	unsigned int m_intRequestType;
	unsigned int m_intOpsType;
	unsigned int m_intReturnCode;

	unsigned int Analyze();

	unsigned int ConvertToASCII(char* cInput);
};

#endif
