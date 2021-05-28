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

FCall<int, CStudioHdr*, const char*> fStudio_LookupSequence;
FCall<int, const CStudioHdr*, const char*> fStudio_FindAttachment;
FCall<int, CStudioHdr*, int, int> fStudio_SelectWeightedSequence;

int CBaseAnimatingHack::offset_HandleAnimEvent = 0;

VCall<void> CBaseAnimatingHack::vStudioFrameAdvance;
VCall<void, CBaseAnimatingHack*> CBaseAnimatingHack::vDispatchAnimEvents;
VCall<bool, int, matrix3x4_t> CBaseAnimatingHack::vGetAttachment;
MCall<float, CStudioHdr*, int> CBaseAnimatingHack::mSequenceDuration;
MCall<void, int> CBaseAnimatingHack::mResetSequence;
MCall<int, CStudioHdr*, const char*> CBaseAnimatingHack::mLookupPoseParameter;
MCall<float, int> CBaseAnimatingHack::mGetPoseParameter;
MCall<float, CStudioHdr*, int, float> CBaseAnimatingHack::mSetPoseParameter;

// TO-DO: Delete
MCall<bool, int, Vector&, QAngle&> mGetAttachment;

// Members
DEFINEVAR(CBaseAnimatingHack, m_pStudioHdr);
DEFINEVAR(CBaseAnimatingHack, m_OnIgnite);
DEFINEVAR(CBaseAnimatingHack, m_nSequence);
DEFINEVAR(CBaseAnimatingHack, m_flModelScale);

bool CBaseAnimatingHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		fStudio_LookupSequence.Init(config, "Studio_LookupSequence");
		fStudio_FindAttachment.Init(config, "Studio_FindAttachment");
		fStudio_SelectWeightedSequence.Init(config, "Studio_SelectWeightedSequence");

		mSequenceDuration.Init(config, "CBaseAnimating::SequenceDuration");
		mResetSequence.Init(config, "CBaseAnimating::ResetSequence");
		mLookupPoseParameter.Init(config, "CBaseAnimating::LookupPoseParameter");
		mSetPoseParameter.Init(config, "CBaseAnimating::SetPoseParameter");
		mGetPoseParameter.Init(config, "CBaseAnimating::GetPoseParameter");
		mGetAttachment.Init(config, "CBaseAnimating::GetAttachment");

		vGetAttachment.Init(config, "CBaseAnimating::GetAttachment");
		vStudioFrameAdvance.Init(config, "CBaseAnimating::StudioFrameAdvance");
		vDispatchAnimEvents.Init(config, "CBaseAnimating::DispatchAnimEvents");
	}
	catch (const std::exception& e)
	{
		// Could use strncpy, but compiler complains
		snprintf(error, maxlength, "%s", e.what());
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

void CBaseAnimatingHack::StudioFrameAdvance(void)
{
	vStudioFrameAdvance(this);
}

void CBaseAnimatingHack::DispatchAnimEvents(CBaseAnimatingHack* other)
{
	vDispatchAnimEvents(this, other);
}

int CBaseAnimatingHack::LookupSequence(const char* label)
{
	return fStudio_LookupSequence(GetModelPtr(), label);
}

int CBaseAnimatingHack::SelectWeightedSequence(Activity activity)
{
	return fStudio_SelectWeightedSequence(GetModelPtr(), activity, GetSequence());
}

int CBaseAnimatingHack::SelectWeightedSequence(Activity activity, int cursequence)
{
	return fStudio_SelectWeightedSequence(GetModelPtr(), activity, cursequence);
}

int CBaseAnimatingHack::LookupAttachment(const char* name)
{
	return fStudio_FindAttachment(GetModelPtr(), name) + 1;
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

float CBaseAnimatingHack::SequenceDuration(CStudioHdr* studio, int sequence)
{
	return mSequenceDuration(this, studio, sequence);
}

void CBaseAnimatingHack::ResetSequence(int sequence)
{
	mResetSequence(this, sequence);
}

bool CBaseAnimatingHack::GetAttachment(const char *szName, Vector &absOrigin, QAngle &absAngles)
{																
	return GetAttachment(LookupAttachment(szName), absOrigin, absAngles);
}

bool CBaseAnimatingHack::GetAttachment(int iAttachment, Vector &absOrigin, QAngle &absAngles)
{
	return mGetAttachment(this, iAttachment, absOrigin, absAngles);
	/*matrix3x4_t attachmentToWorld;

	bool bRet = GetAttachment(iAttachment, attachmentToWorld);
	MatrixAngles(attachmentToWorld, absAngles, absOrigin);
	return bRet;*/
}

bool CBaseAnimatingHack::GetAttachment(int iAttachment, matrix3x4_t& attachmentToWorld)
{
	return vGetAttachment(this, iAttachment, attachmentToWorld);
}

int CBaseAnimatingHack::LookupPoseParameter(CStudioHdr* studio, const char* name)
{
	return mLookupPoseParameter(this, studio, name);
}

float CBaseAnimatingHack::GetPoseParameter(int parameter)
{
	return mGetPoseParameter(this, parameter);
}

float CBaseAnimatingHack::SetPoseParameter(CStudioHdr* studio, int parameter, float val)
{
	return mSetPoseParameter(this, studio, parameter, val);
}