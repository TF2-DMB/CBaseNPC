#ifndef _CUSTOM_FACTORY_H
#define _CUSTOM_FACTORY_H

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>

#include "helpers.h"
#include "baseentity.h"

extern IServerTools *servertools;

class CustomFactory : public IEntityFactory
{
public:
	CustomFactory(const char* classname, MCall<void>* entConstructor);
    ~CustomFactory();
	virtual IServerNetworkable* Create(const char*) override final;
	virtual void Create_Extra(CBaseEntity* ent) {};
	virtual void Create_PostConstructor(CBaseEntity* ent) {};
	virtual void Destroy(IServerNetworkable*) override final;

	MCall<void>* constructor;
};

#endif