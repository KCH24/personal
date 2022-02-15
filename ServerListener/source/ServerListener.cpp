// ServerListener.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WS2tcpip.h>
#include <stdio.h>

#include "ServerListener.h"
#include "HttpMessageAnalyzer.h"
#include "DbOperations.h"

#pragma comment (lib, "ws2_32.lib")

CServerListener::CServerListener()
{}

CServerListener::~CServerListener()
{}

unsigned int CServerListener::ConstructServerReply(unsigned int intDbOpsReturnCode, char* cInputServerReply, char** cServerReply)
{
	char* cReplyData;

	cReplyData = (char*)malloc(SIZE_DATA_4096);
	memset(cReplyData, 0x00, SIZE_DATA_4096);

	strcpy_s(cReplyData, SIZE_DATA_4096, "HTTP/1.1 200 OK\n");
	strcat_s(cReplyData, SIZE_DATA_4096, "Server: Todolist Server\n");
	strcat_s(cReplyData, SIZE_DATA_4096, "Access-Control-Allow-Origin: *\n");
	
	sprintf_s(cReplyData + strlen(cReplyData) + 1, SIZE_DATA_4096, "Content-Length: %d\n", strlen(cInputServerReply));
	
	strcat_s(cReplyData, SIZE_DATA_4096, "Connection: close\n");
	strcat_s(cReplyData, SIZE_DATA_4096, "Content-Type: text/plain\n\n");
	strcat_s(cReplyData, SIZE_DATA_4096, cInputServerReply);

	memcpy(*cServerReply, cReplyData, strlen(cReplyData));

	if (cReplyData != NULL)
	{
		free(cReplyData);
		cReplyData = NULL;
	}

	return ERR_SUCCESS;
}

int main()
{
	long intBindResult;
	CServerListener srvlisten;

    std::cout << "++++++++++++++++++++++++++++++ START TODO LIST SERVER ++++++++++++++++++++++++++++++\n";

	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't initialize winsock! Quitting" << std::endl;
		return 1;
	}

	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return 1;
	}

	//bind the socket to an ip address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	hint.sin_port = htons(54000);

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//tell winsock the socket is for listening
//	listen(listening, SOMAXCONN);
	if (listen(listening, 1) == SOCKET_ERROR)
	{
		std::cerr << "Socket listen error : " << WSAGetLastError() << std::endl;
		closesocket(listening);
		WSACleanup();
		return 1;
	}

	//wait for a connection
	
	SOCKET clientsocket;
	char recvbuf[SIZE_DATA_4096];
	char *sendbuf;
	int clientsize;

	while (true)
	{
		sockaddr_in client;
		clientsize = sizeof(client);
		clientsocket = accept(listening, (sockaddr*)&client, &clientsize);
		if (clientsocket == INVALID_SOCKET)
		{
			std::cerr << "Can't create a socket! Quitting" << std::endl;
			return 1;
		}
		
		char host[NI_MAXHOST];
		char service[NI_MAXHOST];

		ZeroMemory(host,NI_MAXHOST);
		ZeroMemory(service, NI_MAXHOST);

		if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0)==0)
		{
			std::cout << host << " connected on port " << service << " success." << std::endl;
		}
		else
		{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
		}

		//while loop: accept and echo mesage back to client
		ZeroMemory(recvbuf, 4096);

		//wait for client to send data
		int bytesReceived = recv(clientsocket, recvbuf, SIZE_DATA_4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(), Quitting" << std::endl;
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}

		//Echo message back to client
		if (strcmp(recvbuf, "\r\n") != 0 && bytesReceived > 0)
		{
			CHttpMessageAnalyzer httpmsg(recvbuf, bytesReceived);
			CDbOperations dbops(httpmsg.GetReturnCode(), httpmsg.GetRequestOpsData(), strlen(httpmsg.GetRequestOpsData()));
			if (httpmsg.GetReturnCode() != OPS_TYPE_INVALID && dbops.GetReturnCode() != ERR_REQ_OPS_INVALID)
			{
				switch (httpmsg.GetOpsType())
				{
				case OPS_TYPE_INSERT:
					dbops.AddNewRecord();
					break;
				case OPS_TYPE_UPDATE:
					dbops.UpdateRecord();
					break;
				case OPS_TYPE_VIEW:
					dbops.ViewRecord();
					break;
				case OPS_TYPE_DELETE:
					dbops.DeleteRecord();
					break;
				default:
					return ERR_DB_OPS_INVALID;
					break;
				}

				sendbuf = (char*)malloc(SIZE_DATA_2048);
				memset(sendbuf, 0x00, SIZE_DATA_2048);

				srvlisten.ConstructServerReply(dbops.GetReturnCode(), dbops.GetServerReply(), &sendbuf);
				send(clientsocket, sendbuf, strlen(sendbuf), 0);
			}
		}

		//close the socket
		closesocket(clientsocket);
	}

	//close listening socket
	closesocket(listening);

	//shutdown winsock
	WSACleanup();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
