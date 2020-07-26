#include "sourcesdk/baseanimatingoverlay.h"
#include <smsdk_ext.h>

extern CGlobalVars* gpGlobals;
DEFINEVAR(CBaseAnimatingOverlayHack, m_AnimOverlay);

bool CBaseAnimatingOverlayHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	// Any entity that inherits CBaseAnimatingOverlay is good
	// To-do: Should this be moved to gamedata?
	BEGIN_VAR("cycler_flex");
	OFFSETVAR_SEND(CBaseAnimatingOverlay, m_AnimOverlay);
	END_VAR;
	return true;
}

CAnimationLayer::CAnimationLayer()
{
	Init(NULL);
}

void CAnimationLayer::Init(CBaseAnimatingOverlayHack* pOverlay)
{
	m_pOwnerEntity = pOverlay;
	m_fFlags = 0;
	m_flWeight = 0;
	m_flCycle = 0;
	m_flPrevCycle = 0;
	m_bSequenceFinished = false;
	m_nActivity = ACT_INVALID;
	m_nSequence = 0;
	m_nPriority = 0;
	m_nOrder.Set(CBaseAnimatingOverlayHack::MAX_OVERLAYS);

	m_flBlendIn = 0.0;
	m_flBlendOut = 0.0;

	m_flKillRate = 100.0;
	m_flKillDelay = 0.0;
	m_flPlaybackRate = 1.0;
	m_flLastEventCheck = 0.0;
	m_flLastAccess = gpGlobals->curtime;
	m_flLayerAnimtime = 0;
	m_flLayerFadeOuttime = 0;
}

bool CAnimationLayer::IsAbandoned(void)
{
	if (IsActive() && !IsAutokill() && !IsKillMe() && m_flLastAccess > 0.0 && (gpGlobals->curtime - m_flLastAccess > 0.2))
		return true;
	else
		return false;
}

void CAnimationLayer::MarkActive(void)
{
	m_flLastAccess = gpGlobals->curtime;
}

// Dumb and straigth copy of sdk2013
int CBaseAnimatingOverlayHack::AddGestureSequence(int sequence, bool autokill /*= true*/)
{
	int i = AddLayeredSequence(sequence, 0);
	// No room?
	if (IsValidLayer(i))
	{
		SetLayerAutokill(i, autokill);
	}

	return i;
}

int CBaseAnimatingOverlayHack::AddGestureSequence(int nSequence, float flDuration, bool autokill)
{
	int iLayer = AddGestureSequence(nSequence, autokill);

	if (iLayer >= 0 && flDuration > 0)
	{
		auto overlay = m_AnimOverlay();
		overlay->Element(iLayer).m_flPlaybackRate = SequenceDuration(nSequence) / flDuration;
	}
	return iLayer;
}

int CBaseAnimatingOverlayHack::AddGesture(Activity activity, bool autokill)
{
	if (IsPlayingGesture(activity))
	{
		return FindGestureLayer(activity);
	}

	int seq = SelectWeightedSequence(activity);
	if (seq <= 0)
	{
		return -1;
	}

	auto overlay = m_AnimOverlay();
	int i = AddGestureSequence(seq, autokill);
	if (i != -1)
	{
		overlay->Element(i).m_nActivity = activity;
	}

	return i;
}


int CBaseAnimatingOverlayHack::AddGesture(Activity activity, float flDuration, bool autokill)
{
	int iLayer = AddGesture(activity, autokill);
	SetLayerDuration(iLayer, flDuration);

	return iLayer;
}


void CBaseAnimatingOverlayHack::SetLayerDuration(int iLayer, float flDuration)
{
	if (IsValidLayer(iLayer) && flDuration > 0)
	{
		CAnimationLayer* layer = GetAnimOverlay(iLayer);
		layer->m_flPlaybackRate = SequenceDuration(layer->m_nSequence) / flDuration;
	}
}

float CBaseAnimatingOverlayHack::GetLayerDuration(int iLayer)
{
	if (IsValidLayer(iLayer))
	{
		CAnimationLayer layer = m_AnimOverlay()->Element(iLayer);
		if (layer.m_flPlaybackRate != 0.0f)
		{
			return (1.0 - layer.m_flCycle) * SequenceDuration(layer.m_nSequence) / layer.m_flPlaybackRate;
		}
		return SequenceDuration(layer.m_nSequence);
	}
	return 0.0;
}

bool CBaseAnimatingOverlayHack::IsPlayingGesture(Activity activity)
{
	return FindGestureLayer(activity) != -1 ? true : false;
}

void CBaseAnimatingOverlayHack::RestartGesture(Activity activity, bool addifmissing /*=true*/, bool autokill /*=true*/)
{
	int idx = FindGestureLayer(activity);
	if (idx == -1)
	{
		if (addifmissing)
		{
			AddGesture(activity, autokill);
		}
		return;
	}

	CAnimationLayer* layer = GetAnimOverlay(idx);
	layer->m_flCycle = 0.0f;
	layer->m_flPrevCycle = 0.0f;
	layer->m_flLastEventCheck = 0.0f;
}

void CBaseAnimatingOverlayHack::RemoveGesture(Activity activity)
{
	int iLayer = FindGestureLayer(activity);
	if (iLayer == -1)
		return;

	RemoveLayer(iLayer);
}

void CBaseAnimatingOverlayHack::RemoveAllGestures(void)
{
	auto overlay = m_AnimOverlay();
	for (int i = 0; i < overlay->Count(); i++)
	{
		RemoveLayer(i);
	}
}

void CBaseAnimatingOverlayHack::SetLayerCycle(int iLayer, float flCycle)
{
	if (!IsValidLayer(iLayer))
		return;

	CAnimationLayer* layer = GetAnimOverlay(iLayer);
	if (!layer->m_bLooping)
	{
		flCycle = clamp(flCycle, 0.0f, 1.0f);
	}
	layer->m_flCycle = flCycle;
	layer->MarkActive();
}

void CBaseAnimatingOverlayHack::SetLayerCycle(int iLayer, float flCycle, float flPrevCycle)
{
	if (!IsValidLayer(iLayer))
		return;

	CAnimationLayer* layer = GetAnimOverlay(iLayer);
	if (!layer->m_bLooping)
	{
		flCycle = clamp(flCycle, 0.0f, 1.0f);
		flPrevCycle = clamp(flPrevCycle, 0.0f, 1.0f);
	}
	layer->m_flCycle = flCycle;
	layer->m_flPrevCycle = flPrevCycle;
	layer->m_flLastEventCheck = flPrevCycle;
	layer->MarkActive();
}

void CBaseAnimatingOverlayHack::SetLayerCycle(int iLayer, float flCycle, float flPrevCycle, float flLastEventCheck)
{
	if (!IsValidLayer(iLayer))
		return;

	CAnimationLayer* layer = GetAnimOverlay(iLayer);
	if (!layer->m_bLooping)
	{
		flCycle = clamp(flCycle, 0.0f, 1.0f);
		flPrevCycle = clamp(flPrevCycle, 0.0f, 1.0f);
	}

	layer->m_flCycle = flCycle;
	layer->m_flPrevCycle = flPrevCycle;
	layer->m_flLastEventCheck = flLastEventCheck;
	layer->MarkActive();
}

float CBaseAnimatingOverlayHack::GetLayerCycle(int iLayer)
{
	if (!IsValidLayer(iLayer))
		return 0.0;

	auto overlay = m_AnimOverlay();
	return overlay->Element(iLayer).m_flCycle;
}

void CBaseAnimatingOverlayHack::SetLayerPlaybackRate(int iLayer, float flPlaybackRate)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	overlay->Element(iLayer).m_flPlaybackRate = flPlaybackRate;
}

void CBaseAnimatingOverlayHack::SetLayerWeight(int iLayer, float flWeight)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	flWeight = clamp(flWeight, 0.0f, 1.0f);
	overlay->Element(iLayer).m_flWeight = flWeight;
	overlay->Element(iLayer).MarkActive();
}

float CBaseAnimatingOverlayHack::GetLayerWeight(int iLayer)
{
	if (!IsValidLayer(iLayer))
		return 0.0;

	auto overlay = m_AnimOverlay();
	return overlay->Element(iLayer).m_flWeight;
}

void CBaseAnimatingOverlayHack::SetLayerBlendIn(int iLayer, float flBlendIn)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	overlay->Element(iLayer).m_flBlendIn = flBlendIn;
}

void CBaseAnimatingOverlayHack::SetLayerBlendOut(int iLayer, float flBlendOut)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	overlay->Element(iLayer).m_flBlendOut = flBlendOut;
}

void CBaseAnimatingOverlayHack::SetLayerAutokill(int iLayer, bool bAutokill)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	if (bAutokill)
	{
		overlay->Element(iLayer).m_fFlags |= ANIM_LAYER_AUTOKILL;
	}
	else
	{
		overlay->Element(iLayer).m_fFlags &= ~ANIM_LAYER_AUTOKILL;
	}
}

void CBaseAnimatingOverlayHack::SetLayerLooping(int iLayer, bool bLooping)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	overlay->Element(iLayer).m_bLooping = bLooping;
}

void CBaseAnimatingOverlayHack::SetLayerNoRestore(int iLayer, bool bNoRestore)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	if (bNoRestore)
	{
		overlay->Element(iLayer).m_fFlags |= ANIM_LAYER_DONTRESTORE;
	}
	else
	{
		overlay->Element(iLayer).m_fFlags &= ~ANIM_LAYER_DONTRESTORE;
	}
}

Activity CBaseAnimatingOverlayHack::GetLayerActivity(int iLayer)
{
	if (!IsValidLayer(iLayer))
	{
		return ACT_INVALID;
	}

	auto overlay = m_AnimOverlay();
	return overlay->Element(iLayer).m_nActivity;
}

int CBaseAnimatingOverlayHack::GetLayerSequence(int iLayer)
{
	if (!IsValidLayer(iLayer))
	{
		return ACT_INVALID;
	}

	auto overlay = m_AnimOverlay();
	return overlay->Element(iLayer).m_nSequence;
}

void CBaseAnimatingOverlayHack::RemoveLayer(int iLayer, float flKillRate, float flKillDelay)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	if (flKillRate > 0)
	{
		overlay->Element(iLayer).m_flKillRate = overlay->Element(iLayer).m_flWeight / flKillRate;
	}
	else
	{
		overlay->Element(iLayer).m_flKillRate = 100;
	}

	overlay->Element(iLayer).m_flKillDelay = flKillDelay;

	overlay->Element(iLayer).KillMe();
}

void CBaseAnimatingOverlayHack::FastRemoveLayer(int iLayer)
{
	if (!IsValidLayer(iLayer))
		return;

	auto overlay = m_AnimOverlay();
	for (int j = 0; j < overlay->Count(); j++)
	{
		if ((overlay->Element(j).IsActive()) && overlay->Element(j).m_nOrder > overlay->Element(iLayer).m_nOrder)
		{
			overlay->Element(j).m_nOrder--;
		}
	}
	overlay->Element(iLayer).Init(this);
}

CAnimationLayer* CBaseAnimatingOverlayHack::GetAnimOverlay(int iIndex)
{
	auto overlay = m_AnimOverlay();
	iIndex = clamp(iIndex, 0, overlay->Count() - 1);

	return &(overlay->Element(iIndex));
}


void CBaseAnimatingOverlayHack::SetNumAnimOverlays(int num)
{
	auto overlay = m_AnimOverlay();
	if (overlay->Count() < num)
	{
		overlay->AddMultipleToTail(num - overlay->Count());
	}
	else if (overlay->Count() > num)
	{
		overlay->RemoveMultiple(num, overlay->Count() - num);
	}
}

bool CBaseAnimatingOverlayHack::HasActiveLayer(void)
{
	auto overlay = m_AnimOverlay();
	for (int j = 0; j < overlay->Count(); j++)
	{
		if (overlay->Element(j).IsActive())
			return true;
	}

	return false;
}

int	CBaseAnimatingOverlayHack::FindGestureLayer(Activity activity)
{
	auto overlay = m_AnimOverlay();
	for (int i = 0; i < overlay->Count(); i++)
	{
		CAnimationLayer *layer = GetAnimOverlay(i);
		if (!(layer->IsActive()))
			continue;

		if (layer->IsKillMe())
			continue;

		if (layer->m_nActivity == ACT_INVALID)
			continue;

		if (layer->m_nActivity == activity)
			return i;
	}

	return -1;
}

void CBaseAnimatingOverlayHack::SetLayerPriority(int iLayer, int iPriority)
{
	if (!IsValidLayer(iLayer))
	{
		return;
	}

	auto overlay = m_AnimOverlay();
	if (overlay->Element(iLayer).m_nPriority == iPriority)
	{
		return;
	}

	// look for an open slot and for existing layers that are lower priority
	int i;
	for (i = 0; i < overlay->Count(); i++)
	{
		if (overlay->Element(i).IsActive())
		{
			if (overlay->Element(i).m_nOrder > overlay->Element(iLayer).m_nOrder)
			{
				overlay->Element(i).m_nOrder--;
			}
		}
	}

	int iNewOrder = 0;
	for (i = 0; i < overlay->Count(); i++)
	{
		if (i != iLayer && overlay->Element(i).IsActive())
		{
			if (overlay->Element(i).m_nPriority <= iPriority)
			{
				iNewOrder = MAX(iNewOrder, overlay->Element(i).m_nOrder + 1);
			}
		}
	}

	for (i = 0; i < overlay->Count(); i++)
	{
		if (i != iLayer && overlay->Element(i).IsActive())
		{
			if (overlay->Element(i).m_nOrder >= iNewOrder)
			{
				overlay->Element(i).m_nOrder++;
			}
		}
	}

	overlay->Element(iLayer).m_nOrder = iNewOrder;
	overlay->Element(iLayer).m_nPriority = iPriority;
	overlay->Element(iLayer).MarkActive();

	return;
}

int CBaseAnimatingOverlayHack::AllocateLayer(int iPriority)
{
	int i;

	// look for an open slot and for existing layers that are lower priority
	int iNewOrder = 0;
	int iOpenLayer = -1;
	int iNumOpen = 0;
	auto overlay = m_AnimOverlay();
	for (i = 0; i < overlay->Count(); i++)
	{
		if (overlay->Element(i).IsActive())
		{
			if (overlay->Element(i).m_nPriority <= iPriority)
			{
				iNewOrder = MAX(iNewOrder, overlay->Element(i).m_nOrder + 1);
			}
		}
		else if (overlay->Element(i).IsDying())
		{
			// skip
		}
		else if (iOpenLayer == -1)
		{
			iOpenLayer = i;
		}
		else
		{
			iNumOpen++;
		}
	}

	if (iOpenLayer == -1)
	{
		if (overlay->Count() >= MAX_OVERLAYS)
		{
			return -1;
		}

		iOpenLayer = overlay->AddToTail();
		overlay->Element(iOpenLayer).Init(this);
	}

	// make sure there's always an empty unused layer so that history slots will be available on the client when it is used
	if (iNumOpen == 0)
	{
		if (overlay->Count() < MAX_OVERLAYS)
		{
			i = overlay->AddToTail();
			overlay->Element(i).Init(this);
		}
	}

	for (i = 0; i < overlay->Count(); i++)
	{
		if (overlay->Element(i).m_nOrder >= iNewOrder && overlay->Element(i).m_nOrder < MAX_OVERLAYS)
		{
			overlay->Element(i).m_nOrder++;
		}
	}

	overlay->Element(iOpenLayer).m_fFlags = ANIM_LAYER_ACTIVE;
	overlay->Element(iOpenLayer).m_nOrder = iNewOrder;
	overlay->Element(iOpenLayer).m_nPriority = iPriority;
	overlay->Element(iOpenLayer).MarkActive();

	return iOpenLayer;
}

int	CBaseAnimatingOverlayHack::AddLayeredSequence(int sequence, int iPriority)
{
	int i = AllocateLayer(iPriority);
	// No room?
	if (IsValidLayer(i))
	{
		CAnimationLayer* layer = GetAnimOverlay(i);
		layer->m_flCycle = 0;
		layer->m_flPrevCycle = 0;
		layer->m_flPlaybackRate = 1.0;
		layer->m_nActivity = ACT_INVALID;
		layer->m_nSequence = sequence;
		layer->m_flWeight = 1.0f;
		layer->m_flBlendIn = 0.0f;
		layer->m_flBlendOut = 0.0f;
		layer->m_bSequenceFinished = false;
		layer->m_flLastEventCheck = 0;
		layer->m_bLooping = false;//((GetSequenceFlags(GetModelPtr(), sequence) & STUDIO_LOOPING) != 0);
	}

	return i;
}

bool CBaseAnimatingOverlayHack::IsValidLayer(int iLayer)
{
	auto overlay = m_AnimOverlay();
	return (iLayer >= 0 && iLayer < overlay->Count() && overlay->Element(iLayer).IsActive());
}