#ifndef CBASENPC_INTERNAL_H_SHARED
#define CBASENPC_INTERNAL_H_SHARED

#include <vector>
#include <utlvector.h>
#include <map>
#include <stack>

#include "shared/cbasenpc.h"
#include "extension.h"
#include "sourcesdk/customfactory.h"
#include "cbasenpc_locomotion.h"

class CBaseNPC_Entity;

// ======================
// CBaseNPC for plugins
// ======================

class CBaseNPC_Body : public IBody
{
public:
	CBaseNPC_Body(INextBot* bot);

	virtual void Update() override final;
	virtual bool StartActivity(Activity aAct, unsigned int iFlags) override final;
	virtual float GetHullWidth() const override final;
	virtual float GetHullHeight() const override final;
	virtual float GetStandHullHeight() const override final;
	virtual float GetCrouchHullHeight() const override final;
	virtual const Vector& GetHullMins() const override final;
	virtual const Vector& GetHullMaxs() const override final;
	virtual unsigned int GetSolidMask() const override final;

	Vector m_vecBodyMins;
	Vector m_vecBodyMaxs;
};

class NextBotCombatCharacter;
class CBaseNPCPluginActionFactory;
class CBaseNPCIntention;

class CBaseNPC_Entity : public NextBotCombatCharacter
{
public:
	class CBaseNPC : public CExtNPC
	{
	public:
		CBaseNPC(NextBotCombatCharacter* ent);
		~CBaseNPC();

		std::vector<int> m_hookids;
		CBaseNPC_Locomotion* m_pMover;
		CBaseNPC_Body* m_pBody;
		char m_type[64];

		void Hook_Spawn(void);
		ILocomotion* Hook_GetLocomotionInterface(void) const;
		IBody* Hook_GetBodyInterface(void) const;
	};

	CBaseNPC_Entity::CBaseNPC* GetNPC(void);
	void BotUpdateOnRemove(void);
	void BotThink(void);
	void BotSpawn(void);
#ifdef __linux__
	void Hook_Destructor( void );
#else
	void Hook_Destructor( unsigned int flags );
#endif
	void BotDestroy(void);
	int OnTakeDamage(const CTakeDamageInfo& info);
	int OnTakeDamage_Alive(const CTakeDamageInfo& info);

	// Debugging for Behavior
	bool IsDebugging(unsigned int type) { return MyNextBotPointer()->IsDebugging(type); }
	const char* GetDebugIdentifier() { return MyNextBotPointer()->GetDebugIdentifier(); }
	void DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... );
	void DisplayDebugText( const char *text ) { MyNextBotPointer()->DisplayDebugText( text ); };

	static void** vtable;
	static MCall<void> mOriginalSpawn;
	static MCall<void> mOriginalUpdateOnRemove;
	static MCall<int, const CTakeDamageInfo&> mOriginalOnTakeDamage;
	static MCall<int, const CTakeDamageInfo&> mOriginalOnTakeDamage_Alive;
};

class CBaseNPCFactory : public CustomFactory
{
public:
	CBaseNPCFactory();
	virtual ~CBaseNPCFactory();
	virtual size_t GetEntitySize() override final;
	virtual void Create_Extra(CBaseEntityHack* ent) override final;
	virtual void Create_PostConstructor(CBaseEntityHack* ent) override final;
};

extern CBaseNPCFactory* g_pBaseNPCFactory;

#endif
