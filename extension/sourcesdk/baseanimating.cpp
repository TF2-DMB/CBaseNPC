#include "sourcesdk/baseanimating.h"
#include <smsdk_ext.h>
#include <sh_memory.h>
#include <IGameHelpers.h>
#include <variant_t.h>

class CEventAction;
class CBaseEntityOutput
{
public:
	~CBaseEntityOutput();

	void ParseEventAction(const char* EventData);
	void AddEventAction(CEventAction* pEventAction);

	int Save(ISave& save);
	int Restore(IRestore& restore, int elementCount);

	int NumberOfElements(void);

	float GetMaxDelay(void);

	fieldtype_t ValueFieldType() { return m_Value.FieldType(); }

	void FireOutput(variant_t Value, CBaseEntity* pActivator, CBaseEntity* pCaller, float fDelay = 0);

	/// Delete every single action in the action list. 
	void DeleteAllElements(void);

protected:
	variant_t m_Value;
	CEventAction* m_ActionList;
	DECLARE_SIMPLE_DATADESC();

	CBaseEntityOutput() {} // this class cannot be created, only it's children

private:
	CBaseEntityOutput(CBaseEntityOutput&); // protect from accidental copying
};

//-----------------------------------------------------------------------------
// Purpose: parameterless entity event
//-----------------------------------------------------------------------------
class COutputEvent : public CBaseEntityOutput
{
public:
	// void Firing, no parameter
	void FireOutput(CBaseEntity* pActivator, CBaseEntity* pCaller, float fDelay = 0);
};

DEFINEFUNCTION(CBaseAnimatingHack, SequenceDuration, float, (CStudioHdr* studio, int sequence), (studio, sequence));
DEFINEFUNCTION_void(CBaseAnimatingHack, ResetSequence, (int sequence), (sequence));
DEFINEFUNCTION(CBaseAnimatingHack, GetAttachment, bool, (int attachment, Vector& absOrigin, QAngle& absAngles), (attachment, absOrigin, absAngles));
DEFINEFUNCTION(CBaseAnimatingHack, LookupPoseParameter, int, (CStudioHdr* studio, const char* name), (studio, name));
DEFINEFUNCTION(CBaseAnimatingHack, SetPoseParameter, float, (CStudioHdr* studio, int parameter, float value), (studio, parameter, value));
DEFINEFUNCTION(CBaseAnimatingHack, GetPoseParameter, float, (int parameter), (parameter));
DEFINEFUNCTION_virtual_void(CBaseAnimatingHack, StudioFrameAdvance, (), ());
DEFINEFUNCTION_virtual_void(CBaseAnimatingHack, DispatchAnimEvents, (CBaseAnimatingHack* animating), (animating));


int (*Studio_LookupSequence)(CStudioHdr* pstudiohdr, const char* label) = nullptr;
int (*Studio_FindAttachment)(const CStudioHdr* pStudioHdr, const char* pAttachmentName) = nullptr;
int (*Studio_SelectWeightedSequence)(CStudioHdr* pstudiohdr, int activity, int curSequence) = nullptr;

// Vtable
int (CBaseAnimatingHack::CBaseAnimatingHack::offset_HandleAnimEvent) = 0;

// Members
DEFINEVAR(CBaseAnimatingHack, m_pStudioHdr);
DEFINEVAR(CBaseAnimatingHack, m_OnIgnite);
DEFINEVAR(CBaseAnimatingHack, m_nSequence);
DEFINEVAR(CBaseAnimatingHack, m_flModelScale);

bool CBaseAnimatingHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	FINDSIG(config, SetPoseParameter, "CBaseAnimating::SetPoseParameter");
	FINDSIG(config, GetPoseParameter, "CBaseAnimating::GetPoseParameter");
	FINDSIG(config, LookupPoseParameter, "CBaseAnimating::LookupPoseParameter");
	FINDSIG(config, SequenceDuration, "CBaseAnimating::SequenceDuration");
	FINDSIG(config, ResetSequence, "CBaseAnimating::ResetSequence");
	FINDSIG(config, GetAttachment, "CBaseAnimating::GetAttachment");
	FINDVTABLE(config, StudioFrameAdvance, "CBaseAnimating::StudioFrameAdvance");
	FINDVTABLE(config, DispatchAnimEvents, "CBaseAnimating::DispatchAnimEvents");

	if (!config->GetMemSig("Studio_LookupSequence", reinterpret_cast<void**>(&Studio_LookupSequence)))
	{
		snprintf(error, maxlength, "Couldn't locate function Studio_LookupSequence!");
		return false;
	}

	if (!config->GetMemSig("Studio_FindAttachment", reinterpret_cast<void**>(&Studio_FindAttachment)))
	{
		snprintf(error, maxlength, "Couldn't locate function Studio_FindAttachment!");
		return false;
	}

	if (!config->GetMemSig("Studio_SelectWeightedSequence", reinterpret_cast<void**>(&Studio_SelectWeightedSequence)))
	{
		snprintf(error, maxlength, "Couldn't locate function Studio_SelectWeightedSequence!");
		return false;
	}

	if (!config->GetOffset("CBaseAnimating::HandleAnimEvent", &CBaseAnimatingHack::offset_HandleAnimEvent))
	{
		snprintf(error, maxlength, "Failed to retrieve CBaseAnimating::HandleAnimEvent offset!");
		return false;
	}

	// Any entity that inherits CBaseAnimating is good
	BEGIN_VAR("gib");
	OFFSETVAR_DATA(CBaseAnimating, m_OnIgnite);
	OFFSETVAR_SEND(CBaseAnimating, m_nSequence);
	OFFSETVAR_SEND(CBaseAnimating, m_flModelScale);
	// m_pStudioHdr is in front of m_OnIgnite
	VAR_OFFSET_SET(m_pStudioHdr, VAR_OFFSET(m_OnIgnite) + sizeof(COutputEvent));
	END_VAR;

	void* aVal = nullptr;
	if (!config->GetAddress("GetAnimationEvent", &aVal))
	{
		snprintf(error, maxlength, "Failed to retrieve GetAnimationEvent address!");
		return false;
	}

	uint8_t* aGetAnimationEvent = reinterpret_cast<uint8_t*>(aVal);
#ifdef WIN32
	SourceHook::SetMemAccess(aGetAnimationEvent + 0x83, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0x83) = 9999;
#else
	SourceHook::SetMemAccess(aGetAnimationEvent + 0x17E, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0x17E) = 9999;
	SourceHook::SetMemAccess(aGetAnimationEvent + 0xB3, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0xB3) = 9999;
#endif
	return true;
}

int CBaseAnimatingHack::LookupSequence(const char* label)
{
	return (*Studio_LookupSequence)(GetModelPtr(), label);
}

int CBaseAnimatingHack::SelectWeightedSequence(Activity activity)
{
	return (*Studio_SelectWeightedSequence)(GetModelPtr(), activity, GetSequence());
}

int CBaseAnimatingHack::SelectWeightedSequence(Activity activity, int cursequence)
{
	return (*Studio_SelectWeightedSequence)(GetModelPtr(), activity, cursequence);
}

int CBaseAnimatingHack::LookupAttachment(const char* name)
{
	return (Studio_FindAttachment(GetModelPtr(), name))+1;
}

float CBaseAnimatingHack::SetPoseParameter(CStudioHdr* studio, const char* name, float value)
{
	int poseParam = LookupPoseParameter(studio, name);
	return SetPoseParameter(studio, poseParam, value);
}

float CBaseAnimatingHack::GetPoseParameter(const char* name)
{
	return GetPoseParameter(LookupPoseParameter(name));
}

