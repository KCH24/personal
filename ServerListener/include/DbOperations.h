#pragma once
#ifndef __DB_OPERATIONS_H__
#define __DB_OPERATIONS_H__

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "GlobalDefine.h"
#include "ErrorCodeList.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace sql;

class CDbOperations
{
public:
	CDbOperations();
	CDbOperations(unsigned int intMsgAnalyzeReturnCode, char* cRequestMsg, int intMsgSize);
	virtual ~CDbOperations();

	unsigned int AddNewRecord();
	unsigned int UpdateRecord();
	unsigned int ViewRecord();
	unsigned int DeleteRecord();
	char* GetServerReply() { return m_cServerReply; };
	unsigned int GetReturnCode() { return m_intReturnCode; };

private:

	unsigned int m_intDbOpsType;
	char* m_cServerReply;
	int m_intReturnCode;
	
	Connection *dbConn;

	Document jsondoc;

	unsigned int InitiateDbConnection();
	bool CloseDbConnection();
};

#endif
