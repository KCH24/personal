#pragma once
#ifndef __SERVER_LISTENER_H__
#define __SERVER_LISTENER_H__

class CServerListener
{
public:
	CServerListener();
	virtual ~CServerListener();

	unsigned int ConstructServerReply(unsigned int intDbOpsReturnCode, char* cInputServerReply, char** cServerReply);

};

#endif
