#include "natives.hpp"

#include "natives/baseanimating.hpp"
#include "natives/baseanimatingoverlay.hpp"
#include "natives/basecombatcharacter.hpp"
#include "natives/baseentity.hpp"
#include "natives/cbasenpc.hpp"
#include "natives/entityfactory.hpp"
#include "natives/nav.hpp"
#include "natives/nextbot.hpp"
#include "natives/nextbotplayer.hpp"
#include "natives/takedamageinfo.hpp"

#include <ehandle.h>
#include <isaverestore.h>
#include <takedamageinfo.h>
#include <utlmap.h>
#include <CDetour/detours.h>

#include "sourcesdk/baseentity.h"
#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/nav_area.h"

class CTakeDamageInfoHack : public CTakeDamageInfo
{
public:
	inline int GetAttacker() const { return m_hAttacker.IsValid() ? m_hAttacker.GetEntryIndex() : -1; }
	inline int GetInflictor() const { return m_hInflictor.IsValid() ? m_hInflictor.GetEntryIndex() : -1; }
	inline int GetWeapon() const { return m_hWeapon.IsValid() ? m_hWeapon.GetEntryIndex() : -1; }

	inline void SetDamageForce(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamageForce.x = x;
		m_vecDamageForce.y = y;
		m_vecDamageForce.z = z;
	}

	inline void SetDamagePosition(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamagePosition.x = x;
		m_vecDamagePosition.y = y;
		m_vecDamagePosition.z = z;
	}
};

SH_DECL_MANUALHOOK1_void(MEvent_Killed, 0, 0, 0, CTakeDamageInfoHack &);
extern IForward *g_pForwardEventKilled;
extern CUtlMap<int32_t, int32_t> g_EntitiesHooks;

namespace natives {

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

void Event_Killed(CTakeDamageInfoHack &info)
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);
	
	int entity = gamehelpers->EntityToBCompatRef(pEntity);
	int attacker = info.GetAttacker();
	int inflictor = info.GetInflictor();
	float damage = info.GetDamage();
	int damagetype = info.GetDamageType();
	int weapon = info.GetWeapon();
	Vector force = info.GetDamageForce();
	cell_t damageForce[3] = { sp_ftoc(force.x), sp_ftoc(force.y), sp_ftoc(force.z) };
	Vector pos = info.GetDamagePosition();
	cell_t damagePosition[3] = { sp_ftoc(pos.x), sp_ftoc(pos.y), sp_ftoc(pos.z) };
	cell_t res, ret = Pl_Continue;
	
	if (g_pForwardEventKilled != NULL)
	{
		g_pForwardEventKilled->PushCell(entity);
		g_pForwardEventKilled->PushCellByRef(&attacker);
		g_pForwardEventKilled->PushCellByRef(&inflictor);
		g_pForwardEventKilled->PushFloatByRef(&damage);
		g_pForwardEventKilled->PushCellByRef(&damagetype);
		g_pForwardEventKilled->PushCellByRef(&weapon);
		g_pForwardEventKilled->PushArray(damageForce, 3, SM_PARAM_COPYBACK);
		g_pForwardEventKilled->PushArray(damagePosition, 3, SM_PARAM_COPYBACK);
		g_pForwardEventKilled->PushCell(info.GetDamageCustom());
		g_pForwardEventKilled->Execute(&res);
		
		if (res >= ret)
		{
			ret = res;
			if (ret == Pl_Changed)
			{
				CBaseEntity *pEntAttacker = gamehelpers->ReferenceToEntity(attacker);
				if (pEntAttacker)
					info.SetAttacker(pEntAttacker);
				CBaseEntity *pEntInflictor = gamehelpers->ReferenceToEntity(inflictor);
				if (pEntInflictor)
					info.SetInflictor(pEntInflictor);
				info.SetDamage(damage);
				info.SetDamageType(damagetype);
				info.SetWeapon(gamehelpers->ReferenceToEntity(weapon));
				info.SetDamageForce(
					sp_ctof(damageForce[0]),
					sp_ctof(damageForce[1]),
					sp_ctof(damageForce[2]));
				info.SetDamagePosition(
					sp_ctof(damagePosition[0]),
					sp_ctof(damagePosition[1]),
					sp_ctof(damagePosition[2]));
			}
		}

		if (ret >= Pl_Handled)
			RETURN_META(MRES_SUPERCEDE);

		if (ret == Pl_Changed)
			RETURN_META(MRES_HANDLED);
	}
	
	RETURN_META(MRES_IGNORED);
}

cell_t CBaseNPC_GetNextBotOfEntity(IPluginContext *pContext, const cell_t *params) {
	CBaseEntity *pEntity;
	ENTINDEX_TO_CBASEENTITY(params[1], pEntity);
	
	return ptr_toPtrIndex(pEntity->MyNextBotPointer());
}

cell_t CBaseNPC_HookEventKilled(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity;
	ENTINDEX_TO_CBASEENTITY(params[1], pEntity);
	
	auto iIndex = g_EntitiesHooks.Find(gamehelpers->EntityToReference(pEntity));
	if (g_EntitiesHooks.IsValidIndex(iIndex))
		return 1;
	
	int iHookID = SH_ADD_MANUALHOOK(MEvent_Killed, pEntity, Event_Killed, false);
	g_EntitiesHooks.Insert(gamehelpers->EntityToReference(pEntity), iHookID);
	return 1;
}

cell_t Util_ConcatTransforms(IPluginContext* pContext, const cell_t* params) {
	cell_t* inMat1;
	cell_t* inMat2;
	cell_t* outMat;
	pContext->LocalToPhysAddr(params[1], &inMat1);
	pContext->LocalToPhysAddr(params[2], &inMat2);
	pContext->LocalToPhysAddr(params[3], &outMat);

	matrix3x4_t in1;
	matrix3x4_t in2;
	matrix3x4_t out;

	PawnMatrixToMatrix(pContext, inMat1, in1);
	PawnMatrixToMatrix(pContext, inMat2, in2);
	ConcatTransforms(in1, in2, out);
	MatrixToPawnMatrix(pContext, outMat, out);

	return 0;
}


void setup(std::vector<sp_nativeinfo_t>& natives) {

	baseanimating::setup(natives);
	baseanimatingoverlay::setup(natives);
	basecombatcharacter::setup(natives);
	basecombatcharacter::setup(natives);
	baseentity::setup(natives);
	cbasenpc::setup(natives);
	entityfactory::setup(natives);
	nav::setup(natives);
	nextbot::setup(natives);
	nextbotplayer::setup(natives);
	takedamageinfo::setup(natives);

	sp_nativeinfo_t list[] = {
		{"CBaseNPC_GetNextBotOfEntity", &CBaseNPC_GetNextBotOfEntity},
		{"CBaseNPC_HookEventKilled", &CBaseNPC_HookEventKilled},
		{"ConcatTransforms", &Util_ConcatTransforms},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

#ifdef __linux__
SH_DECL_MANUALHOOK0_void(Entity_Dtor, 1, 0, 0);
#else
SH_DECL_MANUALHOOK1_void(Entity_Dtor, 0, 0, 0, unsigned int);
#endif

struct EntityPointersInfo {
	EntityPointersInfo(CBaseEntity* entity) : _gameCreatedNextBot(nullptr) {
		natives::add_ptr(entity);

		_gameCreatedNextBot = entity->MyNextBotPointer();
		if (_gameCreatedNextBot)
		{
			natives::add_ptr(_gameCreatedNextBot);
			// Some interfaces are only created if retrieved
			// create them early, so we don't miss one
			_gameCreatedNextBot->GetLocomotionInterface();
			_gameCreatedNextBot->GetBodyInterface();
			_gameCreatedNextBot->GetIntentionInterface();
			_gameCreatedNextBot->GetVisionInterface();

			for (auto it = _gameCreatedNextBot->FirstContainedResponder(); it; it = _gameCreatedNextBot->NextContainedResponder(it)) {
				natives::add_ptr(it);
				_gameCreatedResponders.push_back(it);
			}
		}

		_hookID = SH_ADD_MANUALHOOK(Entity_Dtor, entity, SH_MEMBER(this, &EntityPointersInfo::Hook_EntityDestructor), false);
	}

#ifdef __linux__
	void Hook_EntityDestructor(void) {
#else
	void Hook_EntityDestructor(unsigned int flags) {
#endif
		CBaseEntity* entity = META_IFACEPTR(CBaseEntity);
		natives::erase_ptr(entity);

		delete this;
	}

	~EntityPointersInfo() {
		if (_gameCreatedNextBot) {
			natives::erase_ptr(_gameCreatedNextBot);
		}

		for (auto responder : _gameCreatedResponders) {
			natives::erase_ptr(responder);
		}

		SH_REMOVE_HOOK_ID(_hookID);
	}

	int _hookID;
	INextBot* _gameCreatedNextBot;
	std::vector<void*> _gameCreatedResponders;
};

CDetour* g_CBaseEntity_PostConstructor = nullptr;
DETOUR_DECL_MEMBER1(CBaseEntity_PostConstructor, void, const char*, name)
{
	DETOUR_MEMBER_CALL(CBaseEntity_PostConstructor)(name);

	new EntityPointersInfo((CBaseEntity*)this);
}

// We're in a lot of troubles if nav areas are added or removed
void ptr_register_navmesh() {
	FOR_EACH_VEC(TheNavAreas, it) {
		auto area = TheNavAreas[it];
		add_ptr(area);

		// Register the ladders
		for (int k = 0; k < (int)CNavLadder::LadderDirectionType::NUM_LADDER_DIRECTIONS; k++) {
			auto& ladderVector = *area->GetLadders( (CNavLadder::LadderDirectionType)k );
			FOR_EACH_VEC(ladderVector, it) {
				add_ptr(ladderVector[it].ladder);
			}
		}
	}
	FOR_EACH_VEC(TheHidingSpots, it) {
		add_ptr(TheHidingSpots[it]);
	}
}

// We're in a lot of troubles if nav areas are added or removed
void ptr_unregister_navmesh() {
	FOR_EACH_VEC(TheNavAreas, it) {
		auto area = TheNavAreas[it];
		erase_ptr(area);	

		for (int k = 0; k < (int)CNavLadder::LadderDirectionType::NUM_LADDER_DIRECTIONS; k++) {
			auto& ladderVector = *area->GetLadders( (CNavLadder::LadderDirectionType)k );
			FOR_EACH_VEC(ladderVector, it) {
				erase_ptr(ladderVector[it].ladder);
			}
		}
	}
	FOR_EACH_VEC(TheHidingSpots, it) {
		erase_ptr(TheHidingSpots[it]);
	}
}

std::unordered_map<const void*, std::uint32_t> g_ptrToIndex;
std::unordered_map<std::uint32_t, const void*> g_indexToPtr;
std::uint32_t g_ptrCounter = 0;

std::uint32_t ptr_toPtrIndex(const void* ptr) {
	if (ptr == nullptr || g_ptrToIndex.find(ptr) == g_ptrToIndex.end()) {
		return 0;
	}
	return g_ptrToIndex[ptr];
}

void* ptrIndex_toPtr(std::uint32_t index) {
	if (index == 0 || g_indexToPtr.find(index) == g_indexToPtr.end()) {
		return nullptr;
	}
	return (void*)g_indexToPtr[index];
}

std::uint32_t add_ptr(const void* ptr) {
	if (g_ptrCounter == 0) {
		g_ptrCounter++;
	}

	g_ptrToIndex[ptr] = g_ptrCounter;
	g_indexToPtr[g_ptrCounter] = ptr;
	return g_ptrCounter;
}

void erase_ptrIndex(std::uint32_t index) {
	const void* ptr = g_indexToPtr[index];

	g_indexToPtr.erase(index);
	g_ptrToIndex.erase(ptr);
}

void erase_ptr(const void* ptr) {
	std::uint32_t index = g_ptrToIndex[ptr];

	g_indexToPtr.erase(index);
	g_ptrToIndex.erase(ptr);
}

void ptr_setup(SourceMod::IGameConfig* config) {
	// Impossible to fail
	int offset = -100000;
	config->GetOffset("CBaseEntity::PostConstructor", &offset);

	auto entity = servertools->CreateEntityByName("info_target");
	void** vtable = *(void***)entity;
	g_CBaseEntity_PostConstructor = DETOUR_CREATE_MEMBER(CBaseEntity_PostConstructor, vtable[offset]);
	g_CBaseEntity_PostConstructor->EnableDetour();
	servertools->RemoveEntityImmediate(entity);
}

}