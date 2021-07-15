#include "customfactory.h"
#include "entityfactorydictionary.h"

CustomFactory::CustomFactory(const char* classname, MCall<void>* entConstructor) : constructor(nullptr)
{
	EntityFactoryDictionaryHack()->m_Factories.Insert( classname, this );
	this->constructor = entConstructor;
}

CustomFactory::~CustomFactory()
{
    EntityFactoryDictionaryHack()->UninstallFactory(this);
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