#ifndef _PLUGINBOT_EXTENSION_H_INCLUDED_
	#define _PLUGINBOT_EXTENSION_H_INCLUDED_
#pragma once

#include <IBinTools.h>
#include <IEngineTrace.h>
#include <convar.h>
#include <utlmap.h>
#include <ISDKHooks.h>
#include <ISDKTools.h>
#include <itoolentity.h>
#include "helpers.h"
#include "shared/npctools.h"
#include <datamap.h>
#include <cstddef>


extern CGlobalVars *gpGlobals;
extern IBinTools *g_pBinTools;
extern ISDKTools *g_pSDKTools;
extern IServerGameEnts *gameents;
extern IdentityType_t g_CoreIdent;
extern IGameConfig *g_pGameConf;
extern IEngineTrace *enginetrace;
extern IServerTools *servertools;

extern IForward *g_pForwardPathCost;

#if SOURCE_ENGINE == SE_TF2
extern HandleType_t HANDLENAME(PluginBotForEachKnownEntity);
#endif
extern HandleType_t HANDLENAME(PluginPathFollower);

extern HandleType_t HANDLENAME(PluginBotReply);
extern HandleType_t HANDLENAME(PluginBotEntityFilter);

extern HandleType_t HANDLENAME(AreasCollector);
extern HandleType_t HANDLENAME(TAreasCollector);

extern HandleType_t g_CellArrayHandle;
extern HandleType_t g_KeyValueType;

extern ConVar* g_cvDeveloper;

class CBaseNPCExt : public SDKExtension, public ISMEntityListener, public IConCommandBaseAccessor
{
	public: // SDKExtension
		virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
		virtual void SDK_OnUnload();
		virtual void SDK_OnAllLoaded();
		//virtual void SDK_OnPauseChange(bool paused);
		virtual bool QueryRunning(char *error, size_t maxlength);
		virtual bool QueryInterfaceDrop(SMInterface *pInterface);
		virtual void NotifyInterfaceDrop(SMInterface *pInterface);
		virtual void OnCoreMapStart(edict_t *pEdictList, int edictCount, int clientMax);
		virtual void OnCoreMapEnd(void);
	public: // ISMEntityListener
		virtual void OnEntityCreated(CBaseEntity* pEntity, const char* classname);
		virtual void OnEntityDestroyed(CBaseEntity* pEntity);
	public: // IConCommandBaseAccessor
		bool RegisterConCommandBase(ConCommandBase *pVar);
	public:
		#if defined SMEXT_CONF_METAMOD
			virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);
			//virtual bool SDK_OnMetamodUnload(char *error, size_t maxlength);
			//virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlength);
		#endif

	bool Initialize(char* error, size_t maxlength, datamap_t* const* dataMaps = nullptr);

	bool GetDataMaps(const char* const* dataMapNames, datamap_t** dataMaps, size_t count); //get datamaps by memory scan

private:
	//initialize hooks and offsets when server is ready in case it isn't, fired only once
	bool Hook_LevelInit(const char* pMapName, const char* pMapEntities, const char* pOldLevel, const char* pLandmarkName, bool loadGame, bool background);
	void TestDummyNPC(); //check if we got all needed interfaces

	int m_iLevelInitHookID = 0;
};

#endif