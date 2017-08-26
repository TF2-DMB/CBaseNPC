#ifndef _UNSORTED_CLASS_
#define _UNSORTED_CLASS_
	
#include <ihandleentity.h>

#define DECLARE_CLASS_NOBASE( className )					typedef className ThisClass;

class CNavArea;
class INextBot;

typedef bool (*ShouldHitFunc_t)( IHandleEntity *pHandleEntity, int contentsMask );

//This function is a hack around CTraceFilterSimple do not edit it! you may create segmentation fault
class CTraceFilterSimpleHack : public CTraceFilter
{
public:
	DECLARE_CLASS_NOBASE( CTraceFilterSimpleHack );
	CTraceFilterSimpleHack( const IHandleEntity *passentity, int collisionGroup, ShouldHitFunc_t pExtraShouldHitCheckFn = NULL );
	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );
	virtual void SetPassEntity( const IHandleEntity *pPassEntity ) { m_pPassEnt = pPassEntity; }
	virtual void SetCollisionGroup( int iCollisionGroup ) { m_collisionGroup = iCollisionGroup; }

	const IHandleEntity *GetPassEntity( void ){ return m_pPassEnt;}
private:
	const IHandleEntity *m_pPassEnt;
	int m_collisionGroup;
	ShouldHitFunc_t m_pExtraShouldHitCheckFunction;
	IPluginFunction *m_pFunc;
public:
	static bool (CTraceFilterSimpleHack::*func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask);
	void SetFunctionPtr(IPluginFunction *pFunc)
	{
		m_pFunc = pFunc;
	}
};

class CBaseCombatCharacter : public IHandleEntity
{
	public:
		const Vector&	GetAbsOrigin( void ) const
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsOrigin", &info);
			
			int offset = info.actual_offset;
			return *(Vector *)((uint8_t *)this + offset);
		}
		const Vector&	GetAbsVelocity( void ) const
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsVelocity", &info);
			
			int offset = info.actual_offset;
			return *(Vector *)((uint8_t *)this + offset);
		}
		void SetAbsVelocity(const Vector &vecVelocity);
		void SetAbsOrigin(const Vector &vecAbsOrigin);
		int GetTeamNumber()
		{
			edict_t *pEdict = gameents->BaseEntityToEdict((CBaseEntity *)this);
			sm_sendprop_info_t info;
			
			ServerClass *sc = pEdict->GetNetworkable()->GetServerClass();
			gamehelpers->FindSendPropInfo(sc->GetName(), "m_iTeamNum", &info);
			
			int offset = info.actual_offset;
			
			int iTeamNum = *(uint8_t *)((unsigned char*)this + offset);
			return iTeamNum;
		}
		int GetMoveType()
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_MoveType", &info);
			
			int offset = info.actual_offset;
			return *((uint8_t *)this + offset);
		}
		float GetModelScale()
		{
			edict_t *pEdict = gameents->BaseEntityToEdict((CBaseEntity *)this);
			sm_sendprop_info_t info;
			
			ServerClass *sc = pEdict->GetNetworkable()->GetServerClass();
			gamehelpers->FindSendPropInfo(sc->GetName(), "m_flModelScale", &info);
			
			int offset = info.actual_offset;
			
			float modelscale = *(float *)((unsigned char*)this + offset);
			return modelscale;
		}
		CNavArea *GetLastKnownArea()
		{
			static ICallWrapper *pCallLastKnownArea = nullptr;
			if (!pCallLastKnownArea)
			{
				PassInfo ret;
				ret.flags = PASSFLAG_BYVAL;
				ret.size = sizeof(CNavArea *);
				ret.type = PassType_Basic;
				
				pCallLastKnownArea = g_pBinTools->CreateVCall(g_iLastKnownAreaOffset, 0, 0, &ret, nullptr, 0);
				if (!pCallLastKnownArea)
					return nullptr;
			}
			unsigned char vstk[sizeof(CBaseCombatCharacter *)];
			unsigned char *vptr = vstk;
			
			*(CBaseCombatCharacter **)vptr = this;

			CNavArea *area = nullptr;
			pCallLastKnownArea->Execute(vstk, &area);

			return area;
		}
};
class CBaseEntityEx
{
	public:
		const Vector&	GetAbsOrigin( void ) const
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsOrigin", &info);
			
			int offset = info.actual_offset;
			return *(Vector *)((uint8_t *)this + offset);
		}
};
class CBasePropDoor
{
	public:
		const Vector&	GetAbsOrigin( void ) const
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsOrigin", &info);
			
			int offset = info.actual_offset;
			return *(Vector *)((uint8_t *)this + offset);
		}
		const QAngle&	GetAbsAngles( void ) const
		{
			sm_datatable_info_t info;
			gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_angAbsRotation", &info);
			
			int offset = info.actual_offset;
			return *(QAngle *)((uint8_t *)this + offset);
		}
};
#endif