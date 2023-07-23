#ifndef H_BASEANIMATING_CBASENPC_
#define H_BASEANIMATING_CBASENPC_
#pragma once
#include "ai_activity.h"
#include "sourcesdk/baseentity.h"

class CStudioHdr;
class COutputEvent;

class CBaseAnimating : public CBaseEntity
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	float GetModelScale() const;

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

	static int offset_HandleAnimEvent;

	static VCall<void> vStudioFrameAdvance;
	void StudioFrameAdvance(void);

	static VCall<void, CBaseAnimating*> vDispatchAnimEvents;
	void DispatchAnimEvents(CBaseAnimating*);

	static MCall<float, CStudioHdr*, int> mSequenceDuration;
	float SequenceDuration(CStudioHdr*, int);

	static MCall<void, int> mResetSequence;
	void ResetSequence(int);

	static VCall< bool, int, matrix3x4_t & > vGetAttachment;
	bool GetAttachment(int, matrix3x4_t&);
	bool GetAttachment(const char*, Vector &, QAngle &);
	bool GetAttachment(int, Vector&, QAngle&);

	static MCall<int, CStudioHdr*, const char*> mLookupPoseParameter;
	int LookupPoseParameter(CStudioHdr*, const char*);

	static MCall<float, int> mGetPoseParameter;
	float GetPoseParameter(int);

	static MCall<float, CStudioHdr*, int, float> mSetPoseParameter;
	float SetPoseParameter(CStudioHdr*, int, float);

	// Members
	DECLAREVAR(COutputEvent, m_OnIgnite);
	DECLAREVAR(CStudioHdr*, m_pStudioHdr);
	DECLAREVAR(int, m_nSequence);
	DECLAREVAR(float, m_flModelScale);
};

inline float CBaseAnimating::GetModelScale() const
{
	return *m_flModelScale();
}

#endif // H_BASEANIMATING_CBASENPC_