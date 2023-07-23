
#ifndef _BASEENTITYOUTPUTHACK_H
#define _BASEENTITYOUTPUTHACK_H

#include <datamap.h>
#include <mempool.h>
#include <isaverestore.h>

#include "variant.h"
#include "helpers.h"


extern CUtlMemoryPool * g_pEntityListPool;
extern ISaveRestoreOps *eventFuncs;

class CEventAction
{
public:
	string_t m_iTarget; // name of the entity(s) to cause the action in
	string_t m_iTargetInput; // the name of the action to fire
	string_t m_iParameter; // parameter to send, 0 if none
	float m_flDelay; // the number of seconds to wait before firing the action
	int m_nTimesToFire; // The number of times to fire this event, or EVENT_FIRE_ALWAYS.

	int m_iIDStamp;	// unique identifier stamp

	CEventAction *m_pNext;

private:
	CEventAction() {} // don't even instantiate this, we can't manage its memory
};

class CBaseEntityOutput
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	void Init();
	void Destroy();

	int NumberOfElements( void );

	float GetMaxDelay( void );

	fieldtype_t ValueFieldType() { return m_Value.fieldType; }

	void DeleteAllElements( void );

public:
	variant_t m_Value;
	CEventAction *m_ActionList;

};

class COutputEvent : public CBaseEntityOutput
{
};

#endif