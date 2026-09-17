#include <memory>

#include "extension.h"
#include <CDetour/detours.h>
#include "helpers.h"
#include "sourcesdk/nav_mesh.h"
#if SOURCE_ENGINE == SE_TF2  
#include "sourcesdk/tf_gamerules.h"  
#endif
#include "sourcesdk/basetoggle.h"
#include "sourcesdk/funcbrush.h"
#include "natives.hpp"
#include <ihandleentity.h>
#include "npc_tools_internal.h"
#include "baseentityoutput.h"
#include "pluginentityfactory.h"
#include "cbasenpc_behavior.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <link.h>
#endif

class CTakeDamageInfoHack;
SH_DECL_MANUALEXTERN1_void(MEvent_Killed, CTakeDamageInfoHack &);
SH_DECL_HOOK6(IServerGameDLL, LevelInit, SH_NOATTRIB, 0, bool, const char *, const char *, const char *, const char *, bool, bool);

CGlobalVars* gpGlobals = nullptr;
IGameConfig* g_pGameConf = nullptr;
IBinTools* g_pBinTools = nullptr;
ISDKTools* g_pSDKTools = nullptr;
ISDKHooks* g_pSDKHooks = nullptr;
IServerGameEnts* gameents = nullptr;
IEngineTrace* enginetrace = nullptr;
IdentityToken_t * g_pCoreIdent = nullptr;
CBaseEntityList* g_pEntityList = nullptr;
IServerTools* servertools = nullptr;
IMDLCache* mdlcache = nullptr;
CSharedEdictChangeInfo* g_pSharedChangeInfo = nullptr;
IStaticPropMgrServer* staticpropmgr = nullptr;
ConVar* sourcemod_version = nullptr;
IBaseNPC_Tools* g_pBaseNPCTools = new BaseNPC_Tools_API;
std::vector<sp_nativeinfo_t> gNatives;

DEFINEHANDLEOBJ(AreasCollector, CUtlVector< CNavArea* >);

ConVar* g_cvDeveloper = nullptr;
extern ConVar* NextBotSpeedLookAheadRange;
extern ConVar* NextBotGoalLookAheadRange;
extern ConVar* NextBotLadderAlignRange;
extern ConVar* NextBotAllowAvoiding;
extern ConVar* NextBotAllowClimbing;
extern ConVar* NextBotAllowGapJumping;
extern ConVar* NextBotDebugClimbing;
extern ConVar* NextBotDebugHistory;
extern ConVar* NextBotPathDrawIncrement;
extern ConVar* NextBotPathSegmentInfluenceRadius;
extern ConVar* NextBotPlayerStop;
extern ConVar* NextBotStop;
extern ConVar* nav_solid_props;
extern ConVar* nb_update_framelimit;
extern ConVar* nb_update_maxslide;

HandleType_t g_KeyValueType;

CBaseNPCExt g_CBaseNPCExt;
SMEXT_LINK(&g_CBaseNPCExt);

IForward *g_pForwardEventKilled = nullptr;

bool (ToolsTraceFilterSimple:: *ToolsTraceFilterSimple::func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask) = nullptr;
CUtlMap<int32_t, int32_t> g_EntitiesHooks;

bool m_bInitialized = false;

namespace
{
enum CBaseNPCDataMapIndex : size_t
{
	DATAMAP_CBASEENTITY,
	DATAMAP_CBASEANIMATING,
	DATAMAP_CBASEANIMATINGOVERLAY,
	DATAMAP_CBASETOGGLE,
	DATAMAP_CFUNCBRUSH,
	DATAMAP_COUNT
};
}

bool CBaseNPCExt::SDK_OnLoad(char* error, size_t maxlength, bool late) {
	char conf_error[255];
	if (!gameconfs->LoadGameConfigFile("cbasenpc", &g_pGameConf, conf_error, sizeof(conf_error))) {
		snprintf(error, maxlength, "FAILED TO LOAD GAMEDATA ERROR: %s", conf_error);
		return false;
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);

	bool bEdictSlotsAreNotAvailable = engine->GetEntityCount() < 1;

	if (bEdictSlotsAreNotAvailable) //we loaded early - can't create edicts to get datamaps from their methods, try to scan memory for datamaps instead
	{
		//datamaps we want to get
		const char* dataMapNames[DATAMAP_COUNT] =
		{
			"CBaseEntity",
			"CBaseAnimating",
			"CBaseAnimatingOverlay",
			"CBaseToggle",
			"CFuncBrush"
		};

		//pointer to datamaps we get will be stored here
		datamap_t* dataMaps[DATAMAP_COUNT] = {};

		if (GetDataMaps(dataMapNames, dataMaps, DATAMAP_COUNT)) //we managed to get all datamaps
		{
			if (!Initialize(error, maxlength, dataMaps)) //still didn't initialize smh
			{
				return false;
			}

			g_pSM->LogMessage(myself, "CBaseNPC initialized from static datamaps without creating edicts.");
		}
		else //at least one datamap wasn't found, it's not good, but still must be initialized, hook LevelInit
		{
			m_iLevelInitHookID = SH_ADD_HOOK(IServerGameDLL, LevelInit, gamedll, SH_MEMBER(this, &CBaseNPCExt::Hook_LevelInit), false);
			g_pSM->LogMessage(myself, "Static datamap scan incomplete, CBaseNPC will load data in LevelInit.");
		}
	}
	else if (!Initialize(error, maxlength)) //we can create edicts but didn't initialized
	{
		g_pSM->LogMessage(myself, "CBaseNPC failed to initialize using edicts!");
		return false;
	}

	g_pForwardEventKilled = forwards->CreateForward("CBaseCombatCharacter_EventKilled", ET_Event, 9, nullptr, Param_Cell, Param_CellByRef, Param_CellByRef, Param_FloatByRef, Param_CellByRef, Param_CellByRef, Param_Array, Param_Array, Param_Cell);

	int iOffset = 0;
	GETGAMEDATAOFFSET("CBaseEntity::Event_Killed", iOffset);
	SH_MANUALHOOK_RECONFIGURE(MEvent_Killed, iOffset, 0, 0);

	CREATEHANDLETYPE(AreasCollector);

	sharesys->AddDependency(myself, "bintools.ext", true, true);
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	sharesys->AddDependency(myself, "sdkhooks.ext", true, true);
	sharesys->RegisterLibrary(myself, "cbasenpc");
	sharesys->AddInterface(myself, g_pBaseNPCTools);
	
	gNatives.reserve(1000);
	natives::setup(gNatives);
	gNatives.push_back({nullptr, nullptr});
	sharesys->AddNatives(myself, gNatives.data());

	g_pSM->LogMessage(myself, "Registered %d natives.", gNatives.size() - 1);

	SetDefLessFunc(g_EntitiesHooks);

	//we can test dummy npc when we have edict slots here
	if (!bEdictSlotsAreNotAvailable)
	{
		TestDummyNPC();
	}
	else if (m_iLevelInitHookID == 0) //we can't test dummy npc and we won't be able to test it in LevelInit
	{
		m_iLevelInitHookID = SH_ADD_HOOK(IServerGameDLL, LevelInit, gamedll, SH_MEMBER(this, &CBaseNPCExt::Hook_LevelInit), false);
	}

	return true;
}

bool CBaseNPCExt::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
	GET_V_IFACE_ANY(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_ANY(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	GET_V_IFACE_ANY(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, mdlcache, IMDLCache, MDLCACHE_INTERFACE_VERSION);

	ConVar_Register(0, this);

	sourcemod_version = g_pCVar->FindVar("sourcemod_version");

	g_cvDeveloper = g_pCVar->FindVar("developer");

	NextBotSpeedLookAheadRange = g_pCVar->FindVar("nb_speed_look_ahead_range");
	NextBotGoalLookAheadRange = g_pCVar->FindVar("nb_goal_look_ahead_range");
	NextBotLadderAlignRange = g_pCVar->FindVar("nb_ladder_align_range");
	NextBotAllowAvoiding = g_pCVar->FindVar("nb_allow_avoiding");
	NextBotAllowClimbing = g_pCVar->FindVar("nb_allow_climbing");
	NextBotAllowGapJumping = g_pCVar->FindVar("nb_allow_gap_jumping");
	NextBotDebugClimbing = g_pCVar->FindVar("nb_debug_climbing");
	NextBotDebugHistory = g_pCVar->FindVar("nb_debug_history");
	NextBotPathDrawIncrement = g_pCVar->FindVar("nb_path_draw_inc");
	NextBotPathSegmentInfluenceRadius = g_pCVar->FindVar("nb_path_segment_influence_radius");
	NextBotPlayerStop = g_pCVar->FindVar("nb_player_stop");
	NextBotStop = g_pCVar->FindVar("nb_stop");
	nav_solid_props = g_pCVar->FindVar("nav_solid_props");
	nb_update_framelimit = g_pCVar->FindVar("nb_update_framelimit");
	nb_update_maxslide = g_pCVar->FindVar("nb_update_maxslide");

	g_pSharedChangeInfo = engine->GetSharedEdictChangeInfo();
	gpGlobals = ismm->GetCGlobals();
	return true;
}

bool CBaseNPCExt::RegisterConCommandBase(ConCommandBase* var) {
	/* Always call META_REGCVAR instead of going through the engine. */
	return META_REGCVAR(var);
}

void CBaseNPCExt::OnCoreMapStart(edict_t* edictlist, int edictCount, int clientMax) {
}

void CBaseNPCExt::OnCoreMapEnd() {
	g_pBaseNPCPluginActionFactories->OnCoreMapEnd();
	g_pPluginEntityFactories->OnCoreMapEnd();
	CNavMesh::OnCoreMapEnd();
}

void CBaseNPCExt::OnEntityCreated(CBaseEntity* pEntity, const char* classname) {
}

void CBaseNPCExt::OnEntityDestroyed(CBaseEntity* pEntity) {
	if (!pEntity) {
		return;
	}

	g_pBaseNPCTools->DeleteNPCByEntIndex(gamehelpers->EntityToBCompatRef(pEntity));

	auto iIndex = g_EntitiesHooks.Find(gamehelpers->EntityToReference(pEntity));
	if (g_EntitiesHooks.IsValidIndex(iIndex)) {
		int iHookID = g_EntitiesHooks.Element(iIndex);
		SH_REMOVE_HOOK_ID(iHookID);
		g_EntitiesHooks.RemoveAt(iIndex);
	}
}

// https://github.com/alliedmodders/sourcemod/blob/6928d21bcf746920b0f2f54e2c28b34097a66be2/core/logic/HandleSys.h#L103
struct QHandleType {
	IHandleTypeDispatch *dispatch;
	unsigned int freeID;
	unsigned int children;
	TypeAccess typeSec;
	HandleAccess hndlSec;
	unsigned int opened;
	std::unique_ptr<std::string> name;
};

// https://github.com/alliedmodders/sourcemod/blob/6928d21bcf746920b0f2f54e2c28b34097a66be2/core/logic/HandleSys.h#L125
struct HandleSystemHack {
	void** vptr;
	void *m_Handles;
	QHandleType *m_Types;
};

void CBaseNPCExt::SDK_OnAllLoaded() {
	SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);
	SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);
	SM_GET_LATE_IFACE(SDKHOOKS, g_pSDKHooks);

	handlesys->FindHandleType("KeyValues", &g_KeyValueType);

	// HACK: Get g_pCoreIdent from KeyValues QHandleType
	// g_KeyValueType is an index of QHandleType array m_Types
	// https://github.com/alliedmodders/sourcemod/blob/6928d21bcf746920b0f2f54e2c28b34097a66be2/core/logic/HandleSys.cpp#L254
	g_pCoreIdent = reinterpret_cast< HandleSystemHack * >(handlesys)->m_Types[g_KeyValueType].typeSec.ident;

	if (g_pSDKHooks) {
		g_pSDKHooks->AddEntityListener(this);
	} 

	g_pEntityList = (CBaseEntityList *)gamehelpers->GetGlobalEntityList();
}

bool CBaseNPCExt::QueryRunning(char* error, size_t maxlength) {
	SM_CHECK_IFACE(BINTOOLS, g_pBinTools);
	SM_GET_LATE_IFACE(SDKHOOKS, g_pSDKHooks);
	return true;
}

bool CBaseNPCExt::QueryInterfaceDrop(SMInterface* interface) {
	if (interface == g_pBinTools) {
		g_pBinTools = nullptr;
		return false;
	}

	if (interface == g_pSDKHooks) {
		g_pSDKHooks = nullptr;
		return false;
	}

	if (interface == g_pSDKTools) {
		g_pSDKTools = nullptr;
		return false;
	}

	return IExtensionInterface::QueryInterfaceDrop(interface);
}

void CBaseNPCExt::NotifyInterfaceDrop(SMInterface* interface) {
	if (strcmp(interface->GetInterfaceName(), SMINTERFACE_BINTOOLS_NAME) == 0) {
		g_pBinTools = nullptr;
	}
}

void CBaseNPCExt::SDK_OnUnload()
{
	if (m_iLevelInitHookID != 0)
	{
		SH_REMOVE_HOOK_ID(m_iLevelInitHookID);
		m_iLevelInitHookID = 0;
	}

	if (m_bInitialized)
	{
		CNavMesh::Unload();
		CBaseEntity::Unload();

		if (g_pForwardEventKilled)
		{
			forwards->ReleaseForward(g_pForwardEventKilled);
			g_pForwardEventKilled = nullptr;
		}

		g_pBaseNPCPluginActionFactories->SDK_OnUnload();
		g_pPluginEntityFactories->SDK_OnUnload();

		delete g_pBaseNPCFactory;
		g_pBaseNPCFactory = nullptr;

		m_bInitialized = false;
	}

	if (g_pGameConf)
	{
		gameconfs->CloseGameConfigFile(g_pGameConf);
		g_pGameConf = nullptr;
	}
	
	if (g_pSDKHooks) {
		g_pSDKHooks->RemoveEntityListener(this);
	}

	FOR_EACH_MAP_FAST(g_EntitiesHooks, iHookID)
		SH_REMOVE_HOOK_ID(iHookID);
}

bool CBaseNPCExt::Hook_LevelInit(const char* pMapName, const char* pMapEntities, const char* pOldLevel, const char* pLandmarkName, bool loadGame, bool background)
{
	if (m_iLevelInitHookID != 0)
	{
		SH_REMOVE_HOOK_ID(m_iLevelInitHookID);
		m_iLevelInitHookID = 0;
	}

	if (m_bInitialized)
	{
		TestDummyNPC();
		RETURN_META_VALUE(MRES_IGNORED, true);
	}

	char error[256] = { 0 };

	if (!Initialize(error, sizeof(error)))
	{
		g_pSM->LogError(myself, "CBaseNPC tried initialization in LevelInit and failed!\n %s", error);

		RETURN_META_VALUE(MRES_IGNORED, true);
	}

	TestDummyNPC();

	RETURN_META_VALUE(MRES_IGNORED, true);
}

bool CBaseNPCExt::Initialize(char* error, size_t maxlength, datamap_t* const* dataMaps)
{
	if (!CBaseEntity::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CBASEENTITY] : nullptr)
		|| !CBaseAnimating::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CBASEANIMATING] : nullptr)
		|| !CBaseAnimatingOverlay::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CBASEANIMATINGOVERLAY] : nullptr)
		|| !CFuncBrush::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CFUNCBRUSH] : nullptr)
		|| !CBaseToggle::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CBASETOGGLE] : nullptr)
		|| !CNavMesh::Init(g_pGameConf, error, maxlength)
		|| !CBaseCombatCharacter::Init(g_pGameConf, error, maxlength)
		|| !ToolsTraceFilterSimple::Init(g_pGameConf, error, maxlength)
#if SOURCE_ENGINE == SE_TF2  
		|| !CTFGameRules::Init(g_pGameConf, error, maxlength)
#endif  
		|| !CBaseEntityOutput::Init(g_pGameConf, error, maxlength, dataMaps ? dataMaps[DATAMAP_CBASEENTITY] : nullptr)
		|| !CBaseNPC_Locomotion::Init(g_pGameConf, error, maxlength)
		|| !ToolsNextBot::Init(g_pGameConf, error, maxlength)
		)
	{
		// Some initialization stages install detours before all later
		// stages have succeeded. Roll them back before SourceMod unloads
		// the extension DLL.
		CNavMesh::Unload();
		CBaseEntity::Unload();
		return false;
	}

	if (!g_pPluginEntityFactories->Init(g_pGameConf, error, maxlength))
	{
		CNavMesh::Unload();
		CBaseEntity::Unload();
		return false;
	}

	if (!g_pBaseNPCPluginActionFactories->Init(g_pGameConf, error, maxlength))
	{
		g_pPluginEntityFactories->SDK_OnUnload();
		CNavMesh::Unload();
		CBaseEntity::Unload();
		return false;
	}

	g_pBaseNPCFactory = new CBaseNPCFactory;

	m_bInitialized = true;

	return true;
}

void CBaseNPCExt::TestDummyNPC()
{
	CBaseNPC_Entity* npc = static_cast<CBaseNPC_Entity*>(servertools->CreateEntityByName("base_npc"));

	if (!npc)
	{
		g_pSM->LogError(myself, "Failed to create dummy NPC!");
		return;
	}
	
	if (npc->GetNPC()->GetID() == INVALID_NPC_ID)
	{
		g_pSM->LogError(myself, "Dummy NPC has no id!");
	}

	INextBot* nb = npc->MyNextBotPointer();

	if (nb == nullptr)
	{
		g_pSM->LogError(myself, "Dummy NPC has no nextbot interface!");
	}

	if (npc->GetNPC()->m_pMover == nullptr || (nb && nb->GetLocomotionInterface() == nullptr))
	{
		g_pSM->LogError(myself, "Dummy NPC has no locomotion interface!");
	}

	if (npc->GetNPC()->m_pBody == nullptr || (nb && nb->GetBodyInterface() == nullptr))
	{
		g_pSM->LogError(myself, "Dummy NPC has no body interface!");
	}

	servertools->RemoveEntityImmediate(npc);
	g_pSM->LogMessage(myself, "Successfully created & destroyed dummy NPC");
}

bool CBaseNPCExt::GetDataMaps(const char* const* dataMapNames, datamap_t** dataMaps, size_t count)
{
	// dataMapNames and dataMaps are parallel arrays. The map found for
	// dataMapNames[i] is written to dataMaps[i]. Results already supplied by the
	// caller are preserved and are not scanned again.
	if (!dataMapNames || !dataMaps || count == 0)
	{
		return false;
	}

	struct ModuleMemoryRegion
	{
		uintptr_t begin;
		uintptr_t end;
		bool readable;
		bool writable;
	};

	// Read only the datamap_t prefix shared by release and debug SDK builds.
	// Debug builds may append bValidityChecked after packed_size. Representing
	// the bool fields as bytes also lets us reject values other than 0 or 1.
	struct DataMapPrefix
	{
		typedescription_t* dataDesc;
		int dataNumFields;
		const char* dataClassName;
		datamap_t* baseMap;
		uint8_t chains_validated;
		uint8_t packed_offsets_computed;
		int packed_size;
	};

	static_assert(offsetof(DataMapPrefix, dataClassName) == offsetof(datamap_t, dataClassName), "Unexpected datamap_t layout");
	static_assert(offsetof(DataMapPrefix, packed_size) == offsetof(datamap_t, packed_size), "Unexpected datamap_t layout");

	std::vector<ModuleMemoryRegion> regions;
	uintptr_t probe = 0;

	// The interface object itself may be outside the server image. Its virtual
	// functions are inside it, so the first vtable entry is used as a reliable
	// address belonging to server.dll or server_srv.so.
	if (gamedll)
	{
		// The virtual function belongs to the server image even if the object is on the heap.
		void** vtable = *reinterpret_cast<void***>(gamedll);
		if (vtable && vtable[0])
		{
			probe = reinterpret_cast<uintptr_t>(vtable[0]);
		}
	}

#ifdef _WIN32
	// Locate the complete PE image through the allocation containing probe.
	// This does not depend on the module filename, load address, or ASLR.
	MEMORY_BASIC_INFORMATION moduleInfo = {};
	if (probe && VirtualQuery(reinterpret_cast<const void*>(probe), &moduleInfo, sizeof(moduleInfo)))
	{
		uintptr_t moduleBase = reinterpret_cast<uintptr_t>(moduleInfo.AllocationBase);
		// Walk this image allocation once; inaccessible and guarded pages are excluded.
		uintptr_t cursor = moduleBase;
		while (moduleBase)
		{
			MEMORY_BASIC_INFORMATION info = {};
			if (!VirtualQuery(reinterpret_cast<const void*>(cursor), &info, sizeof(info)) || info.AllocationBase != moduleInfo.AllocationBase)
			{
				break;
			}

			uintptr_t begin = reinterpret_cast<uintptr_t>(info.BaseAddress);
			if (info.RegionSize > UINTPTR_MAX - begin)
			{
				regions.clear();
				break;
			}

			uintptr_t end = begin + info.RegionSize;
			if (end <= cursor)
			{
				regions.clear();
				break;
			}

			if (info.Type == MEM_IMAGE && info.State == MEM_COMMIT && !(info.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
			{
				DWORD protection = info.Protect & 0xFF;
				bool writable = protection == PAGE_READWRITE || protection == PAGE_WRITECOPY ||
					protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
				bool readable = writable || protection == PAGE_READONLY || protection == PAGE_EXECUTE_READ;
				regions.push_back({begin, end, readable, writable});
			}

			cursor = end;
		}
	}
#elif defined(__linux__)
	// Find the ELF object whose PT_LOAD segment contains probe, then collect all
	// loadable segments of that object and their read/write permissions.
	struct ModuleSearchContext
	{
		uintptr_t probe;
		std::vector<ModuleMemoryRegion>* regions;
	} context = {probe, &regions};

	if (probe)
	{
		// dl_iterate_phdr requires a callback; keep it local to GetDataMaps.
		dl_iterate_phdr([](dl_phdr_info* info, size_t, void* userdata) -> int
		{
			ModuleSearchContext* context = static_cast<ModuleSearchContext*>(userdata);
			bool containsProbe = false;

			for (size_t i = 0; i < info->dlpi_phnum; i++)
			{
				const ElfW(Phdr)& phdr = info->dlpi_phdr[i];
				if (phdr.p_type != PT_LOAD || !phdr.p_memsz)
				{
					continue;
				}

				uintptr_t begin = static_cast<uintptr_t>(info->dlpi_addr) + static_cast<uintptr_t>(phdr.p_vaddr);
				if (context->probe >= begin && context->probe - begin < phdr.p_memsz)
				{
					containsProbe = true;
					break;
				}
			}

			if (!containsProbe)
			{
				return 0;
			}

			for (size_t i = 0; i < info->dlpi_phnum; i++)
			{
				const ElfW(Phdr)& phdr = info->dlpi_phdr[i];
				if (phdr.p_type != PT_LOAD || !phdr.p_memsz)
				{
					continue;
				}

				uintptr_t begin = static_cast<uintptr_t>(info->dlpi_addr) + static_cast<uintptr_t>(phdr.p_vaddr);
				if (phdr.p_memsz > UINTPTR_MAX - begin)
				{
					context->regions->clear();
					return 1;
				}

				uintptr_t end = begin + static_cast<uintptr_t>(phdr.p_memsz);
				context->regions->push_back({begin, end, (phdr.p_flags & PF_R) != 0, (phdr.p_flags & PF_W) != 0});
			}

			return 1;
		}, &context);
	}
#endif

	if (regions.empty())
	{
		g_pSM->LogMessage(myself, "GetDataMaps: failed to locate server module memory regions.");
	}

	// Phase 1: find every occurrence of each requested classname literal in
	// readable, non-writable server memory. One classname may have several
	// literal addresses, so all of them are retained.
	std::unordered_map<uintptr_t, size_t> stringOwners;
	size_t remaining = 0;

	for (size_t i = 0; i < count; i++)
	{
		if (dataMaps[i])
		{
			// Keep results that were populated before this call.
			continue;
		}

		remaining++;
		const char* name = dataMapNames[i];
		if (!name || !name[0])
		{
			continue;
		}

		const size_t nameLength = std::strlen(name);
		for (const ModuleMemoryRegion& region : regions)
		{
			// Classname literals normally live in PE .rdata or ELF .rodata. Linux
			// may place .rodata in the executable segment with .text, so executable
			// memory is allowed here; writable memory is not.
			if (!region.readable || region.writable || region.end - region.begin <= nameLength)
			{
				continue;
			}

			uintptr_t last = region.end - nameLength - 1;
			for (uintptr_t address = region.begin; address <= last; address++)
			{
				const char* candidate = reinterpret_cast<const char*>(address);
				// Include the terminating null so CBaseEntity does not also match
				// the beginning of CBaseEntityOutput.
				if (*candidate == name[0] && std::memcmp(candidate, name, nameLength + 1) == 0)
				{
					stringOwners.emplace(address, i);
				}
			}
		}
	}

	// Phase 2: scan writable server memory once for pointer values equal to the
	// literal addresses found above. A real datamap_t stores such a pointer in
	// dataClassName, so most memory is rejected by one integer lookup.
	for (const ModuleMemoryRegion& region : regions)
	{
		if (!remaining || stringOwners.empty())
		{
			break;
		}

		if (!region.readable || !region.writable || region.end - region.begin < sizeof(uintptr_t))
		{
			continue;
		}

		// Static datamaps are naturally pointer-aligned: four bytes on x86 and
		// eight bytes on x64.
		const uintptr_t alignment = sizeof(uintptr_t);
		uintptr_t first = (region.begin + alignment - 1) & ~(alignment - 1);
		for (uintptr_t address = first; address <= region.end - sizeof(uintptr_t); address += alignment)
		{
			uintptr_t pointerValue = 0;
			std::memcpy(&pointerValue, reinterpret_cast<const void*>(address), sizeof(pointerValue));

			auto iter = stringOwners.find(pointerValue);
			if (iter == stringOwners.end() || dataMaps[iter->second] || address < offsetof(datamap_t, dataClassName))
			{
				continue;
			}

			// address is the matching dataClassName member. Move backwards by its
			// compiler-calculated offset to get a possible ClassName::m_DataMap.
			uintptr_t mapAddress = address - offsetof(datamap_t, dataClassName);
			DataMapPrefix map = {};
			bool valid = true;

			// Phase 3: validate every range before reading it. The three passes cover
			// the candidate, its complete typedescription array, and the prefix of
			// its optional baseMap. Adjacent readable regions are allowed, but an
			// unreadable gap rejects the candidate.
			for (int part = 0; part < 3 && valid; part++)
			{
				uintptr_t cursor = mapAddress;
				size_t bytesLeft = sizeof(DataMapPrefix);
				if (part == 1)
				{
					cursor = reinterpret_cast<uintptr_t>(map.dataDesc);
					bytesLeft = static_cast<size_t>(map.dataNumFields) * sizeof(typedescription_t);
				}
				else if (part == 2)
				{
					if (!map.baseMap)
					{
						continue;
					}

					cursor = reinterpret_cast<uintptr_t>(map.baseMap);
				}

				if (!cursor || bytesLeft > UINTPTR_MAX - cursor)
				{
					valid = false;
					break;
				}

				while (bytesLeft)
				{
					bool foundRegion = false;
					for (const ModuleMemoryRegion& readableRegion : regions)
					{
						if (!readableRegion.readable || cursor < readableRegion.begin || cursor >= readableRegion.end)
						{
							continue;
						}

						size_t available = static_cast<size_t>(readableRegion.end - cursor);
						size_t chunk = bytesLeft < available ? bytesLeft : available;
						cursor += chunk;
						bytesLeft -= chunk;
						foundRegion = true;
						break;
					}

					if (!foundRegion)
					{
						valid = false;
						break;
					}
				}

				if (part == 0 && valid)
				{
					std::memcpy(&map, reinterpret_cast<const void*>(mapAddress), sizeof(map));

					// These inexpensive invariants reject accidental references to the
					// same string before typedescription_t objects are inspected.
					valid = map.dataClassName == reinterpret_cast<const char*>(pointerValue) && map.dataDesc &&
						map.dataNumFields > 0 && map.dataNumFields <= 4096 &&
						map.chains_validated <= 1 && map.packed_offsets_computed <= 1 &&
						map.packed_size >= 0 && map.packed_size <= 16 * 1024 * 1024 &&
						reinterpret_cast<uintptr_t>(map.baseMap) != mapAddress;
				}
			}

			if (!valid)
			{
				continue;
			}

			// Phase 4: sample up to eight field descriptors. This strongly rejects
			// lookalike structures without checking hundreds of fields per candidate.
			int fieldsToCheck = map.dataNumFields < 8 ? map.dataNumFields : 8;
			for (int i = 0; i < fieldsToCheck; i++)
			{
				typedescription_t field = {};
				uintptr_t fieldAddress = reinterpret_cast<uintptr_t>(map.dataDesc) + static_cast<size_t>(i) * sizeof(typedescription_t);
				std::memcpy(&field, reinterpret_cast<const void*>(fieldAddress), sizeof(field));

				// Validate only fieldType. DEFINE_FUNCTION can allocate fieldName
				// dynamically outside the server image in a legitimate datamap.
				if (field.fieldType < FIELD_VOID || field.fieldType >= FIELD_TYPECOUNT)
				{
					valid = false;
					break;
				}
			}

			if (!valid)
			{
				continue;
			}

			// Phase 5: publish the validated result. Fill duplicate requests for the
			// same classname while preserving values supplied by the caller.
			const char* foundName = dataMapNames[iter->second];
			for (size_t i = 0; i < count; i++)
			{
				if (!dataMaps[i] && dataMapNames[i] && std::strcmp(dataMapNames[i], foundName) == 0)
				{
					dataMaps[i] = reinterpret_cast<datamap_t*>(mapAddress);
					remaining--;
				}
			}

			if (!remaining)
			{
				break;
			}
		}
	}

	for (size_t i = 0; i < count; i++)
	{
		// Include null results in the log so an incomplete early scan can be
		// diagnosed before initialization falls back to LevelInit.
		g_pSM->LogMessage(myself, "GetDataMaps: %s -> %p", dataMapNames[i] ? dataMapNames[i] : "<null>", static_cast<void*>(dataMaps[i]));
	}

	// Partial results remain in dataMaps, but edict-free initialization is safe
	// only when every requested map was resolved.
	return remaining == 0;
}

// Definitions required by the SDK timer declarations. The extension only uses
// their layout and timing helpers, so an empty datamap is sufficient here.
#if SOURCE_ENGINE == SE_BMS
datamap_t IntervalTimer::m_DataMap = { 0, 0, "IntervalTimer", nullptr };

datamap_t* IntervalTimer::GetBaseMap()
{
	return nullptr;
}

datamap_t* IntervalTimer::GetDataDescMap()
{
	return &m_DataMap;
}
#endif

float IntervalTimer::Now( void ) const {
	return gpGlobals->curtime;
}

float CountdownTimer::Now( void ) const {
	return gpGlobals->curtime;
}

bool CGameTrace::DidHitWorld() const {
	return gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity *>(m_pEnt)) == 0;
}

bool CGameTrace::DidHitNonWorldEntity() const {
	return m_pEnt != nullptr && !DidHitWorld();
}

float UTIL_VecToYaw(const Vector& vec) {
	if (vec.y == 0 && vec.x == 0) {
		return 0;
	}

	float yaw = atan2(vec.y, vec.x);

	yaw = RAD2DEG(yaw);

	if (yaw < 0) {
		yaw += 360;
	}

	return yaw;
}
