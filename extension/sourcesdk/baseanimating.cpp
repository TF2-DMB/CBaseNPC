#include "sourcesdk/baseanimating.h"
#include <studio.h>
#include <smsdk_ext.h>
#include <sh_memory.h>
#include <IGameHelpers.h>

#include "baseentityoutput.h"

FCall<int, CStudioHdr*, const char*> fStudio_LookupSequence;
FCall<int, const CStudioHdr*, const char*> fStudio_FindAttachment;
FCall<int, CStudioHdr*, int, int> fStudio_SelectWeightedSequence;
FCall<float, CStudioHdr*, int, const float*> fStudio_Duration;

int CBaseAnimating::offset_HandleAnimEvent = 0;

VCall<void> CBaseAnimating::vStudioFrameAdvance;
VCall<void, CBaseAnimating*> CBaseAnimating::vDispatchAnimEvents;
VCall< bool, int, matrix3x4_t& > CBaseAnimating::vGetAttachment;
MCall<void, int> CBaseAnimating::mResetSequence;
MCall<int, CStudioHdr*, const char*> CBaseAnimating::mLookupPoseParameter;
MCall<float, int> CBaseAnimating::mGetPoseParameter;
MCall<float, CStudioHdr*, int, float> CBaseAnimating::mSetPoseParameter;

// Members
DEFINEVAR(CBaseAnimating, m_pStudioHdr);
DEFINEVAR(CBaseAnimating, m_OnIgnite);
DEFINEVAR(CBaseAnimating, m_nSequence);
DEFINEVAR(CBaseAnimating, m_flModelScale);
DEFINEVAR(CBaseAnimating, m_flPoseParameter);

bool CBaseAnimating::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		fStudio_LookupSequence.Init(config, "Studio_LookupSequence");
		fStudio_FindAttachment.Init(config, "Studio_FindAttachment");
		fStudio_SelectWeightedSequence.Init(config, "Studio_SelectWeightedSequence");
		fStudio_Duration.Init(config, "Studio_Duration");

		mResetSequence.Init(config, "CBaseAnimating::ResetSequence");
		mLookupPoseParameter.Init(config, "CBaseAnimating::LookupPoseParameter");
		mSetPoseParameter.Init(config, "CBaseAnimating::SetPoseParameter");
		mGetPoseParameter.Init(config, "CBaseAnimating::GetPoseParameter");

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

	if (!config->GetOffset("CBaseAnimating::HandleAnimEvent", &CBaseAnimating::offset_HandleAnimEvent))
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
	OFFSETVAR_SEND(CBaseAnimating, m_flPoseParameter);
	END_VAR;

	void* aVal = nullptr;
	if (!config->GetAddress("GetAnimationEvent", &aVal))
	{
		snprintf(error, maxlength, "Failed to retrieve GetAnimationEvent address!");
		return false;
	}

	uint8_t* aGetAnimationEvent = reinterpret_cast<uint8_t*>(aVal);
#ifdef PLATFORM_X64
#ifdef WIN64
#else
	SourceHook::SetMemAccess(aGetAnimationEvent + 0xF2, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0xF2) = 9999;
#endif
#else
#ifdef WIN32
	SourceHook::SetMemAccess(aGetAnimationEvent + 0x83, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0x83) = 9999;
#else
	SourceHook::SetMemAccess(aGetAnimationEvent + 0x17E, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0x17E) = 9999;
	SourceHook::SetMemAccess(aGetAnimationEvent + 0xB3, sizeof(uint32_t), SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
	*(uint32_t*)(aGetAnimationEvent + 0xB3) = 9999;
#endif
#endif
	return true;
}

void CBaseAnimating::StudioFrameAdvance(void)
{
	vStudioFrameAdvance(this);
}

void CBaseAnimating::DispatchAnimEvents(CBaseAnimating* other)
{
	vDispatchAnimEvents(this, other);
}

int CBaseAnimating::LookupSequence(const char* label)
{
	return fStudio_LookupSequence(GetModelPtr(), label);
}

int CBaseAnimating::SelectWeightedSequence(Activity activity)
{
	return fStudio_SelectWeightedSequence(GetModelPtr(), activity, GetSequence());
}

int CBaseAnimating::SelectWeightedSequence(Activity activity, int cursequence)
{
	return fStudio_SelectWeightedSequence(GetModelPtr(), activity, cursequence);
}

int CBaseAnimating::LookupAttachment(const char* name)
{
	return fStudio_FindAttachment(GetModelPtr(), name) + 1;
}

float CBaseAnimating::SetPoseParameter(CStudioHdr* studio, const char* name, float value)
{
	int poseParam = LookupPoseParameter(studio, name);
	return SetPoseParameter(studio, poseParam, value);
}

float CBaseAnimating::GetPoseParameter(const char* name)
{
	return GetPoseParameter(LookupPoseParameter(name));
}

float CBaseAnimating::SequenceDuration(CStudioHdr* pStudioHdr, int iSequence)
{
	if ( !pStudioHdr )
	{
		DevWarning( 2, "CBaseAnimating::SequenceDuration( %d ) NULL pstudiohdr on %s!\n", iSequence, GetClassname() );
		return 0.1;
	}
	if ( !pStudioHdr->SequencesAvailable() )
	{
		return 0.1;
	}
	if (iSequence >= pStudioHdr->GetNumSeq() || iSequence < 0 )
	{
		DevWarning( 2, "CBaseAnimating::SequenceDuration( %d ) out of range\n", iSequence );
		return 0.1;
	}

	return fStudio_Duration(pStudioHdr, iSequence, GetPoseParameterArray());
}

void CBaseAnimating::ResetSequence(int sequence)
{
	mResetSequence(this, sequence);
}

bool CBaseAnimating::GetAttachment(const char *szName, Vector &absOrigin, QAngle &absAngles)
{
	return GetAttachment(LookupAttachment(szName), absOrigin, absAngles);
}

bool CBaseAnimating::GetAttachment(int iAttachment, Vector &absOrigin, QAngle &absAngles)
{
	matrix3x4_t attachmentToWorld;

	bool bRet = GetAttachment(iAttachment, attachmentToWorld);
	MatrixAngles(attachmentToWorld, absAngles, absOrigin);
	return bRet;
}

bool CBaseAnimating::GetAttachment(int iAttachment, matrix3x4_t& attachmentToWorld)
{
	return vGetAttachment(this, iAttachment, attachmentToWorld);
}

int CBaseAnimating::LookupPoseParameter(CStudioHdr* studio, const char* name)
{
	return mLookupPoseParameter(this, studio, name);
}

float CBaseAnimating::GetPoseParameter(int parameter)
{
	return mGetPoseParameter(this, parameter);
}

float CBaseAnimating::SetPoseParameter(CStudioHdr* studio, int parameter, float val)
{
	return mSetPoseParameter(this, studio, parameter, val);
}