#include "entityfactorydictionary.h"

void CEntityFactoryDictionaryHack::UninstallFactory(IEntityFactory* factory)
{
	for (size_t i = 0; i < m_Factories.Count(); i++)
	{
		if (this->m_Factories[i] == factory)
		{
			m_Factories.RemoveAt(i);
		}
	}
}

CEntityFactoryDictionaryHack* EntityFactoryDictionaryHack()
{
	return ((CEntityFactoryDictionaryHack*)servertools->GetEntityFactoryDictionary());
}