#ifndef _DICTIONARY_FACTORY_H
#define _DICTIONARY_FACTORY_H

#include <tier0/platform.h>
#include <tier1/utldict.h>
#include <itoolentity.h>
#include <IEngineTrace.h>
#include <IStaticPropMgr.h>
#include <shareddefs.h>
#include <util.h>

#include "sourcesdk/customfactory.h"

class CEntityFactoryDictionaryHack : public IEntityFactoryDictionary
{
public:
	void UninstallFactory(IEntityFactory*);
	CUtlDict<IEntityFactory *, unsigned short> m_Factories;
};

CEntityFactoryDictionaryHack *EntityFactoryDictionaryHack();

#endif