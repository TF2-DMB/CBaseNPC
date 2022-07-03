#include "customfactory.h"
#include "entityfactorydictionary.h"

#include "pluginentityfactory.h"

CustomFactory::CustomFactory(const char* classname, MCall<void>* entConstructor) : constructor(nullptr)
{
	g_pPluginEntityFactories->InstallGameFactory(classname, this);
	this->constructor = entConstructor;
}

CustomFactory::~CustomFactory()
{
	g_pPluginEntityFactories->RemoveGameFactory(this);
}

IServerNetworkable* CustomFactory::Create(const char* classname)
{
	CBaseEntityHack* pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(this->GetEntitySize());
	this->constructor->operator()(pEnt);
	this->Create_Extra(pEnt);
    pEnt->PostConstructor(classname);
	this->Create_PostConstructor(pEnt);
	return pEnt->NetworkProp();
}

void CustomFactory::Destroy(IServerNetworkable* pNetworkable)
{
	if (pNetworkable)
	{
		pNetworkable->Release();
	}
}