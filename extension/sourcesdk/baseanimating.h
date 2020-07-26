#ifndef H_BASEANIMATING_CBASENPC_
#define H_BASEANIMATING_CBASENPC_
#pragma once
#include "ai_activity.h"
#include "sourcesdk/baseentity.h"

class CStudioHdr;
class COutputEvent;

class CBaseAnimatingHack : public CBaseEntityHack
{
public:
	DECLARE_CLASS_NOBASE(CBaseAnimatingHack);

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	inline CStudioHdr* GetModelPtr() { return *m_pStudioHdr(); };
	inline int GetSequence() { return *m_nSequence(); }

	int LookupSequence(const char* label);

	inline float SequenceDuration() { return SequenceDuration(GetSequence()); }
	inline float SequenceDuration(int iSequence) { return SequenceDuration(GetModelPtr(), iSequence); }

	int SelectWeightedSequence(Activity activity);
	int	SelectWeightedSequence(Activity activity, int curSequence);

	int LookupAttachment(const char* szName);
	inline int	LookupPoseParameter(const char* name) { return LookupPoseParameter(GetModelPtr(), name); }

	float	SetPoseParameter(CStudioHdr* studio, const char* name, float value);
	inline float SetPoseParameter(const char* szName, float flValue) { return SetPoseParameter(GetModelPtr(), szName, flValue); }
	inline float SetPoseParameter(int parameter, float value) { return SetPoseParameter(GetModelPtr(), parameter, value); }

	float	GetPoseParameter(const char* name);

	static int (CBaseAnimatingHack::offset_HandleAnimEvent);

	DECLAREFUNCTION(SequenceDuration, float, (CStudioHdr* studio, int sequence));
	DECLAREFUNCTION(ResetSequence, void, (int sequence));
	DECLAREFUNCTION_virtual(StudioFrameAdvance, void, ());
	DECLAREFUNCTION_virtual(DispatchAnimEvents, void, (CBaseAnimatingHack* animating));
	DECLAREFUNCTION(GetAttachment, bool, (int attachment, Vector& absOrigin, QAngle& absAngles));
	DECLAREFUNCTION(LookupPoseParameter, int, (CStudioHdr* studio, const char* name));
	DECLAREFUNCTION(GetPoseParameter, float, (int parameter));
	DECLAREFUNCTION(SetPoseParameter, float, (CStudioHdr* studio, int parameter, float value));

	// Members
	DECLAREVAR(COutputEvent, m_OnIgnite);
	DECLAREVAR(CStudioHdr*, m_pStudioHdr);
	DECLAREVAR(int, m_nSequence);
};

#endif // H_BASEANIMATING_CBASENPC_