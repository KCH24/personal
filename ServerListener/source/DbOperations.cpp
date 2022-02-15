
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "DbOperations.h"

using namespace std;

//for demonstration only. never save password in the code
const string strServer = "tcp://localhost:3306";
const string strUsername = "root";
const string strPassword = "Foo1234";

CDbOperations::CDbOperations()
{}

CDbOperations::CDbOperations(unsigned int intMsgAnalyzeReturnCode, char* cRequestMsg, int intMsgSize)
{
	if (cRequestMsg != NULL && intMsgAnalyzeReturnCode != OPS_TYPE_INVALID)
	{
		m_intReturnCode = InitiateDbConnection();

		jsondoc.Parse(cRequestMsg);

		m_intReturnCode = ERR_SUCCESS;
	}
	else
	{
		m_intReturnCode = ERR_REQ_OPS_INVALID;
	}
}

CDbOperations::~CDbOperations()
{

}

unsigned int CDbOperations::InitiateDbConnection()
{
	Driver *driver;

	try
	{
		driver = get_driver_instance();
		dbConn = driver->connect(strServer, strUsername, strPassword);
		
		dbConn->setSchema("todolist");
	}
	catch (SQLException ex)
	{
		cout << "Could not connect to server.\nError message : " << ex.what() << std::endl;
		system("pause");
		return ERR_DB_CONNECTION_ERROR;
	}

	return ERR_SUCCESS;
}

unsigned int CDbOperations::AddNewRecord()
{
	PreparedStatement *prpStmt;
	string strTemp;

	prpStmt = dbConn->prepareStatement("INSERT INTO todorecords(descriptions, status) VALUES(?,?)");

	Value& jsonValue = jsondoc["descriptions"];
	prpStmt->setString(1, jsonValue.GetString());

	jsonValue = jsondoc["status"];
	prpStmt->setString(2, jsonValue.GetString());

	prpStmt->execute();
	cout << "*** New record inserted." << endl;

	if (m_cServerReply != NULL)
	{
		free(m_cServerReply);
		m_cServerReply = NULL;
	}

	m_cServerReply = (char*)malloc(SIZE_DATA_64);
	memset(m_cServerReply,0x00,SIZE_DATA_64);
	memcpy(m_cServerReply,"A new record had been inserted!!!", strlen((char*)"A new record had been inserted!!!"));

	delete prpStmt;
}

unsigned int CDbOperations::UpdateRecord()
{
	PreparedStatement *prpStmt;

	prpStmt = dbConn->prepareStatement("UPDATE todorecords SET status=? WHERE id=?");

	Value& jsonValue = jsondoc["status"];
	prpStmt->setString(1, jsonValue.GetString());

	jsonValue = jsondoc["id"];
	prpStmt->setInt(2, atoi(jsonValue.GetString()));

	prpStmt->executeQuery();
	cout << "*** Record " << atoi(jsonValue.GetString()) << " updated." << std::endl;

	if (m_cServerReply != NULL)
	{
		free(m_cServerReply);
		m_cServerReply = NULL;
	}

	m_cServerReply = (char*)malloc(SIZE_DATA_64);
	memset(m_cServerReply, 0x00, SIZE_DATA_64);
	memcpy(m_cServerReply, "A new record had been updated!!!", strlen((char*)"A new record had been updated!!!"));

	delete prpStmt;
}

unsigned int CDbOperations::ViewRecord()
{
	PreparedStatement *prpStmt;
	Statement *stmt;
	ResultSet *result = NULL;

	string strId = "";
	string strStatus = "";
	char* cTemp;

	Value& jsonValue = jsondoc["id"];
	if (!jsonValue.IsNull())
	{
		strId = jsonValue.GetString();
	}

	jsonValue = jsondoc["status"];
	if (!jsonValue.IsNull())
	{
		strStatus = jsonValue.GetString();
	}

	if (strId == "" && strStatus == "")
	{
		stmt = dbConn->createStatement();
		result = stmt->executeQuery("SELECT * FROM todorecords");

		delete stmt;
	}
	else if (strId != "" && strStatus == "")
	{
		prpStmt = dbConn->prepareStatement("SELECT * FROM todorecords WHERE id=?");
		prpStmt->setInt(1, atoi(strId.c_str()));
		result = prpStmt->executeQuery();

		delete prpStmt;
	}
	else if (strId == "" && strStatus != "")
	{
		prpStmt = dbConn->prepareStatement("SELECT * FROM todorecords WHERE status=?");
		prpStmt->setString(1, strStatus);
		result = prpStmt->executeQuery();

		delete prpStmt;
	}
	else
	{
		cout << "XXX Invalid database query !!!" << endl;
		m_intReturnCode = ERR_DB_INVALID_QUERY;
	}

	m_cServerReply = (char*)malloc(SIZE_DATA_2048);
	memset(m_cServerReply, 0x00, SIZE_DATA_2048);
	if (result != NULL)
	{
		cout << "========== ID ========== descriptions ========== status ==========" << endl;

		sprintf_s(m_cServerReply, SIZE_DATA_2048, "%s", "{\n\t\"records\":\n\t[");

		cTemp = (char*)malloc(SIZE_DATA_1024);
		memset(cTemp, 0x00, SIZE_DATA_1024);

		while (result->next())
		{
			sprintf_s(cTemp, SIZE_DATA_1024, "\n\t\t{\n\t\t\t\"id\":\"%s\",\n\t\t\t\"descriptions\":\"%s\",\n\t\t\t\"status\":\"%s\"\n\t\t}",
						(char*)result->getString(1).c_str(), (char*)result->getString(2).c_str(), (char*)result->getString(3).c_str());

			strcat_s(m_cServerReply, SIZE_DATA_2048, cTemp);
			if (!result->isLast())
			{
				strcat_s(m_cServerReply, SIZE_DATA_2048, (char*)",\n");
			}

			memset(cTemp, 0x00, strlen(cTemp) + 1);

			cout << result->getString(1) << ", ";
			cout << result->getString(2) << ", ";
			cout << result->getString(3) << endl;
		}
		cout << "==================================================================\n" << endl;
		strcat_s(m_cServerReply, SIZE_DATA_2048, "\n\t]\n}");

		if (cTemp != NULL)
		{
			free(cTemp);
			cTemp = NULL;
		}

		delete result;
	}
	else
	{
		sprintf_s(m_cServerReply, SIZE_DATA_128, "%s", "Invalid database operation !!!");
	}
}

unsigned int CDbOperations::DeleteRecord()
{
	PreparedStatement *prpStmt;
	string strMsg;
	bool bException = false;

	prpStmt = dbConn->prepareStatement("DELETE FROM todorecords WHERE id=?");

	Value& jsonValue = jsondoc["id"];
	prpStmt->setInt(1, atoi(jsonValue.GetString()));

	try
	{
		prpStmt->executeQuery();
		cout << "*** Record deleted" << endl;
	}
	catch (SQLException ex)
	{
		bException = true;
		cout << "XXX " << ex.what() << endl;
		strMsg = ex.what();
	}

	if (m_cServerReply != NULL)
	{
		free(m_cServerReply);
		m_cServerReply = NULL;
	}

	m_cServerReply = (char*)malloc(SIZE_DATA_256);
	memset(m_cServerReply, 0x00, SIZE_DATA_256);
	if (bException)
	{
		memcpy(m_cServerReply, strMsg.c_str(), strMsg.length());
	}
	else
	{
		memcpy(m_cServerReply, "A record had been deleted!!!", strlen((char*)"A new record had been deleted!!!"));
	}

	delete prpStmt;
}

bool CDbOperations::CloseDbConnection()
{
	dbConn->close();

	if (dbConn->isClosed())
	{
		delete dbConn;

		return ERR_SUCCESS;
	}

	delete dbConn;
	return ERR_DB_CLOSE_FAILED;
}

/*
int DbOperationsTest()
{
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::PreparedStatement *pstmt;
	sql::ResultSet *result;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}

	//please create database "quickstartdb" ahead of time
	con->setSchema("todolist");

//	stmt = con->createStatement();
//	stmt->execute("DROP TABLE IF EXISTS inventory");
//	cout << "Finished dropping table (if existed)" << std::endl;
//	stmt->execute("CREATE TABLE inventory (id serial PRIMARY KEY, name VARCHAR(50), quantity INTEGER);");
//	cout << "Finished creating table" << std::endl;
//	delete stmt;

//	pstmt = con->prepareStatement("INSERT INTO todorecords(descriptions, status) VALUES(?,?)");
//	pstmt->setString(1, "insert records 1 from C++ program");
//	pstmt->setString(2, "ongoing");
//	pstmt->execute();
//	cout << "One row inserted." << std::endl;
//
//	pstmt->setString(1, "Success insert record 1 from c++ program");
//	pstmt->setString(2, "ongoing");
//	pstmt->execute();
//	cout << "One row inserted." << std::endl;
//
//	pstmt->setString(1, "Complete insert records from c++ program");
//	pstmt->setString(2, "ongoing");
//	pstmt->execute();
//	cout << "One row inserted." << std::endl;
//
//	delete pstmt;

	//Display record
	stmt = con->createStatement();
	result = stmt->executeQuery("SELECT * FROM todorecords");

	cout << "Return result from query, SELECT * FROM todorecords" << std::endl;
	cout << "========== ID ========== descriptions ========== status ==========" << std::endl;
	while (result->next())
	{
		cout << result->getString(1) << "\t\t";
		cout << result->getString(2) << "\t\t";
		cout << result->getString(3) << std::endl;
	}

	delete result;
	delete stmt;

	//Update data
	pstmt = con->prepareStatement("UPDATE todorecords SET status=? WHERE id=?");
	pstmt->setString(1, "completed");
	pstmt->setInt(2, 10004);
	pstmt->executeQuery();
	cout << "Record 10004 updated" << std::endl;

	pstmt->setString(1, "completed");
	pstmt->setInt(2, 10005);
	pstmt->executeQuery();
	cout << "Record 10005 updated" << std::endl << std::endl;;

	//Display updated record
	stmt = con->createStatement();
	result = stmt->executeQuery("SELECT * FROM todorecords");

	cout << "Return result from query, SELECT * FROM todorecords" << std::endl;
	cout << "========== ID ========== descriptions ========== status ==========" << std::endl;
	while (result->next())
	{
		cout << result->getString(1) << "\t\t";
		cout << result->getString(2) << "\t\t";
		cout << result->getString(3) << std::endl;
	}

	delete result;
	delete stmt;
	delete con;
	system("pause");
	return 0;
}
*/