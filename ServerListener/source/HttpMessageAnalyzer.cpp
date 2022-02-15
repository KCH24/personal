#include <stdio.h>
#include <iostream>
#include "HttpMessageAnalyzer.h"
#include "GlobalDefine.h"

using namespace std;

CHttpMessageAnalyzer::CHttpMessageAnalyzer()
{}

CHttpMessageAnalyzer::CHttpMessageAnalyzer(char* cReceivedMessage, unsigned int intMessageSize)
{
	if (cReceivedMessage != NULL)
	{
		cMessage = (char*)malloc(SIZE_DATA_4096);
		memset(cMessage,0x00,SIZE_DATA_4096);

		memcpy(cMessage,cReceivedMessage,intMessageSize);

		m_intReturnCode = Analyze();
	}
}

CHttpMessageAnalyzer::~CHttpMessageAnalyzer()
{}

bool CHttpMessageAnalyzer::IsHex(char cValue)
{
	if ((cValue >= '0' && cValue <= '9') || (cValue >= 'a' && cValue <= 'f') || (cValue >= 'A' && cValue <= 'F'))
	{
		return true;
	}

	return false;
}

unsigned int CHttpMessageAnalyzer::ConvertToASCII(char* cInput)
{
	int intHex;

	if (cInput == NULL)
	{
		return ERR_BUFFER_IS_NULL;
	}

	if (strlen(cInput) < 3)
	{
		return ERR_ASCII_INV_DATA;
	}

	if (cInput[1] < '2' || cInput[1] > '7' || !IsHex(cInput[1]))
	{
		return ERR_ASCII_CONVERT_FAIL;
	}

	sscanf_s(cInput+1, "%02X", &intHex);

	return intHex;
}

unsigned int CHttpMessageAnalyzer::Analyze()
{
	int intMsgLen = strlen(cMessage);
	int intDelimPos = 0;
	int intRet = 0, j=0;
	bool bPause = false;

	char *cPch;
	char *cCompare;
	char *cConvertData;

	m_intRequestType = HTTP_REQUEST_TYPE_INVALID;
	m_intOpsType = OPS_TYPE_INVALID;

	if (cMessage != NULL)
	{
		if (_strnicmp(cMessage,"GET ",4) == 0)
		{
			m_intRequestType = HTTP_REQUEST_TYPE_GET;
		}
		else if (_strnicmp(cMessage,"POST ",5)==0)
		{
			m_intRequestType = HTTP_REQUEST_TYPE_POST;
		}
		else
		{
			return m_intRequestType;
		}
	}
	else
	{
		return ERR_BUFFER_IS_NULL;
	}

	cCompare = (char*)malloc(SIZE_DATA_32);
	memset(cCompare, 0x00, SIZE_DATA_32);

	cPch = strchr(cMessage, '/');
	if (cPch != NULL)
	{
		intDelimPos = cPch - cMessage + 1;

		cPch = NULL;
		cPch = strchr(cMessage + intDelimPos, '/');
		if (cPch != NULL)
		{
			memcpy(cCompare, cMessage + intDelimPos, ((cPch - cMessage + 1) - intDelimPos - 1));
			if (strcmp(cCompare,"insertrecord")==0)
			{
				m_intOpsType = OPS_TYPE_INSERT;
			}
			else if (strcmp(cCompare, "updaterecord") == 0)
			{
				m_intOpsType = OPS_TYPE_UPDATE;
			}
			else if (strcmp(cCompare,"viewrecord") == 0)
			{
				m_intOpsType = OPS_TYPE_VIEW;
			}
			else if (strcmp(cCompare, "deleterecord") == 0)
			{
				m_intOpsType = OPS_TYPE_DELETE;
			}
			else
			{
				return m_intOpsType;
			}
			intDelimPos = cPch - cMessage + 1;

			cPch = NULL;

			free(cCompare);
			cCompare = NULL;
		}
	}

	cPch = strchr(cMessage+intDelimPos,' ');
	if (cPch != NULL)
	{
		//buffer to store converted data
		cConvertData = (char*)malloc(intMsgLen);
		memset(cConvertData, 0x00, intMsgLen);
		memcpy(cConvertData, cMessage+intDelimPos, (cPch-cMessage+1) - intDelimPos);

		intMsgLen = strlen(cConvertData);
		m_cRequest = (char*)malloc(intMsgLen);
		memset(m_cRequest, 0x00, intMsgLen);

		//buffer use to get hex
		cCompare = (char*)malloc(4);
		memset(cCompare, 0x00, 4);

		for (int i=0; i<intMsgLen; i++)
		{
			if (cConvertData[i] == '%')
			{
				sprintf_s(cCompare, 4, "%c%c%c", cConvertData[i], cConvertData[i + 1], cConvertData[i + 2]);

				m_cRequest[j] = ConvertToASCII(cCompare);
				j++;
				i += 2;
			}
			else
			{
				m_cRequest[j] = cConvertData[i];
				j++;
			}
		}
	}
	else
	{
		return ERR_REQ_PARAM_NOT_FOUND;
	}

	j = 0;
}