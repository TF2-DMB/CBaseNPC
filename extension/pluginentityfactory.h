#ifndef CPLUGINENTITYFACTORY_H
#define CPLUGINENTITYFACTORY_H

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>

#include "helpers.h"

enum PluginEntityFactoryDeriveType_t
{
    NONE,
    BASECLASS,
    CLASSNAME
};

enum PluginEntityFactoryDeriveFromBaseType_t
{
    ENTITY,
    NPC
};

struct PluginEntityFactoryDeriveInfo_t
{
    PluginEntityFactoryDeriveType_t m_DeriveFrom;

    PluginEntityFactoryDeriveFromBaseType_t m_BaseType;
    std::string m_iBaseClassname;
    bool m_bBaseEntityServerOnly;
};

class CPluginEntityFactory : public IEntityFactory
{
public:
    std::string m_iClassname;
    IPluginFunction *m_pConstructor;
    PluginEntityFactoryDeriveInfo_t m_Derive;
    bool m_bInstalled;

    CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor);
    ~CPluginEntityFactory();
    void Install();
    void Uninstall();
    virtual IServerNetworkable* Create(const char*) override final;
    virtual size_t GetEntitySize() override final;
    virtual void Destroy(IServerNetworkable*) override final;

    static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

protected:
    IEntityFactory * GetBaseFactory();

    CPluginEntityFactory* m_pBasePluginEntityFactory;
    CUtlVector< CPluginEntityFactory* > m_ChildFactories;

    void SetBasePluginEntityFactory(CPluginEntityFactory* pFactory);

private:
    static void OnFactoryInstall( CPluginEntityFactory* pFactory );
    static void OnFactoryUninstall( CPluginEntityFactory* pFactory );
};

class CPluginEntityFactoryHandler : public IHandleTypeDispatch
{
public:
    void OnHandleDestroy(HandleType_t type, void * object);
};

extern HandleType_t g_PluginEntityFactoryHandle;
extern CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

#endif