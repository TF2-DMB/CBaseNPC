
#ifndef H_SERVERCLASS_CBASENPC_
#define H_SERVERCLASS_CBASENPC_

#include <server_class.h>

class ServerClassHack : public ServerClass
{
public:
	void Init( const char *pNetworkName, SendTable *pTable );

	void AddToGlobalList();
	void RemoveFromGlobalList();
};

#endif