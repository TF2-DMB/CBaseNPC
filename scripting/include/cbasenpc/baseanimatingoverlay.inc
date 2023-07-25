#if defined _CBASENPC_BASEANIMATING_OVERLAY_INC_
 #endinput
#endif
#define _CBASENPC_BASEANIMATING_OVERLAY_INC_

#define ANIM_LAYER_ACTIVE		0x0001
#define ANIM_LAYER_AUTOKILL		0x0002
#define ANIM_LAYER_KILLME		0x0004
#define ANIM_LAYER_DONTRESTORE	0x0008
#define ANIM_LAYER_CHECKACCESS	0x0010
#define ANIM_LAYER_DYING		0x0020
#define ANIM_LAYER_NOEVENTS         0x0040

#define MAX_OVERLAYS			15

#pragma deprecated Use CAnimationLayer instead
enum //CAnimationLayer 
{ 
    m_fFlags = 0,               //0x0000  
    m_bSequenceFinished = 4,    //0x0004  
    m_bLooping = 6,             //0x0006  
    m_nSequence = 8,            //0x0008  
    m_flCycle = 12,             //0x000C  
    m_flPrevCycle = 16,         //0x0010  
    m_flWeight = 20,            //0x0014  
    m_flPlaybackRate = 24,      //0x0018  
    m_flBlendIn = 28,           //0x001C  
    m_flBlendOut = 32,          //0x0020  
    m_flKillRate = 36,          //0x0024  
    m_flKillDelay = 40,         //0x0028  
    m_flLayerAnimtime = 44,     //0x002C  
    m_flLayerFadeOuttime = 48,  //0x0030  
    m_nActivity = 52,           //0x0034  
    m_nPriority = 56,           //0x0038  
    m_nOrder = 60,              //0x003C  
    m_flLastEventCheck = 64,    //0x0040  
    m_flLastEventAccess = 68,   //0x0044  
    m_pOwnerEntity = 72,        //0x0048  
     
    CAnimationLayer_Size = 76   //0x004C 
}; //Size=0x004C 

methodmap CAnimationOverlay  
{
	#pragma deprecated Use CAnimationLayer instead
	public CAnimationOverlay(Address address)
	{
		return view_as<CAnimationOverlay>(address);
	}
	
	property Address Address  
	{
		public get()  
		{
			return view_as<Address>(this); 
		}
	}
	
	property bool isNull
	{
		public get()  
		{
			return this.Address == Address_Null; 
		}
	}

	public any Get(int iOffset) 
	{ 
		return LoadFromAddress(this.Address + view_as<Address>(iOffset), NumberType_Int32); 
	}
	
	public void Set(int iOffset, any iValue) 
	{ 
		StoreToAddress(this.Address + view_as<Address>(iOffset), iValue, NumberType_Int32); 
	}
	
	#pragma deprecated Use CAnimationLayer.IsActive() instead
	public bool IsActive()    { return ((this.Get(m_fFlags) & ANIM_LAYER_ACTIVE)   != 0); } 
	#pragma deprecated Use CAnimationLayer.IsAutokill() instead
	public bool IsAutokill()  { return ((this.Get(m_fFlags) & ANIM_LAYER_AUTOKILL) != 0); } 
	#pragma deprecated Use CAnimationLayer.IsKillMe() instead
	public bool IsKillMe()    { return ((this.Get(m_fFlags) & ANIM_LAYER_KILLME)   != 0); } 
	#pragma deprecated Use CAnimationLayer.IsDying() instead
	public bool IsDying()     { return ((this.Get(m_fFlags) & ANIM_LAYER_DYING)    != 0); } 
	#pragma deprecated Use CAnimationLayer.NoEvents() instead
	public bool NoEvents()    { return ((this.Get(m_fFlags) & ANIM_LAYER_NOEVENTS) != 0); }
	#pragma deprecated Use CAnimationLayer.KillMe() instead
	public void KillMe()      { this.Set(m_fFlags, this.Get(m_fFlags) | ANIM_LAYER_KILLME); } 
	#pragma deprecated Use CAnimationLayer.AutoKill() instead
	public void AutoKill()    { this.Set(m_fFlags, this.Get(m_fFlags) | ANIM_LAYER_AUTOKILL); }
	#pragma deprecated Use CAnimationLayer.Dying() instead
	public void Dying()       { this.Set(m_fFlags, this.Get(m_fFlags) | ANIM_LAYER_DYING);  } 
	#pragma deprecated Use CAnimationLayer.Dead() instead
	public void Dead()        { this.Set(m_fFlags, this.Get(m_fFlags) & ~ANIM_LAYER_DYING); }
	#pragma deprecated Use CAnimationLayer.IsAlive() instead
	public bool IsAlive()         { int iFlags = this.Get(m_fFlags); return (((iFlags & ANIM_LAYER_ACTIVE) != 0) || ((iFlags & ANIM_LAYER_KILLME) == 0)); }

	#pragma deprecated Use CAnimationLayer.m_nSequence instead
	public int GetLayerSequence() { return (this.Get(m_nSequence)); }
};

methodmap CAnimationLayer < CAnimationOverlay
{
	property int m_fFlags
	{
		public native get();
		public native set(int value);
	}

	property bool m_bSequenceFinished
	{
		public native get();
		public native set(bool value);
	}

	property bool m_bLooping
	{
		public native get();
		public native set(bool value);
	}

	property int m_nSequence
	{
		public native get();
		public native set(int value);
	}

	property float m_flCycle
	{
		public native get();
		public native set(float value);
	}

	property float m_flPrevCycle
	{
		public native get();
		public native set(float value);
	}

	property float m_flWeight
	{
		public native get();
		public native set(float value);
	}

	property float m_flPlaybackRate
	{
		public native get();
		public native set(float value);
	}

	property float m_flBlendIn
	{
		public native get();
		public native set(float value);
	}

	property float m_flBlendOut
	{
		public native get();
		public native set(float value);
	}

	property float m_flKillRate
	{
		public native get();
		public native set(float value);
	}

	property float m_flKillDelay
	{
		public native get();
		public native set(float value);
	}

	property float m_flLayerAnimtime
	{
		public native get();
		public native set(float value);
	}

	property float m_flLayerFadeOuttime
	{
		public native get();
		public native set(float value);
	}

	property Activity m_nActivity
	{
		public native get();
		public native set(Activity value);
	}

	property int m_nPriority
	{
		public native get();
		public native set(int value);
	}

	property int m_nOrder
	{
		public native get();
		public native set(int value);
	}

	property float m_flLastEventCheck
	{
		public native get();
		public native set(float value);
	}

	property float m_flLastAccess
	{
		public native get();
		public native set(float value);
	}

	property int m_pOwnerEntity
	{
		public native get();
		public native set(int value);
	}

	public bool IsActive()    { return (this.m_fFlags & ANIM_LAYER_ACTIVE) != 0; } 
	public bool IsAutokill()  { return (this.m_fFlags & ANIM_LAYER_AUTOKILL) != 0; } 
	public bool IsKillMe()    { return (this.m_fFlags & ANIM_LAYER_KILLME) != 0; } 
	public bool IsDying()     { return (this.m_fFlags & ANIM_LAYER_DYING) != 0; } 
	public bool NoEvents()    { return (this.m_fFlags & ANIM_LAYER_NOEVENTS) != 0; }
	public void KillMe()      { this.m_fFlags |= ANIM_LAYER_KILLME; } 
	public void AutoKill()    { this.m_fFlags |= ANIM_LAYER_AUTOKILL; }
	public void Dying()       { this.m_fFlags |= ANIM_LAYER_DYING; } 
	public void Dead()        { this.m_fFlags &= ~ANIM_LAYER_DYING; }
	public bool IsAlive()     { return (((this.m_fFlags & ANIM_LAYER_ACTIVE) != 0) || ((this.m_fFlags & ANIM_LAYER_KILLME) == 0)); }
};

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public CBaseAnimatingOverlay(int entity) 
	{
		return view_as<CBaseAnimatingOverlay>(entity);
	}
	
	/**
	 * Adds the given sequence as gesture.
	 *
	 * @param sequence	Animation sequence index.
	 * @param duration	Duration of the gesture.
	 * @param autokill	Whether or not, the gesture should be removed upon animation completion.
	 * @return		Layer index.
	 * @error 		Invalid entity.
	 */
	public native int AddGestureSequence(int sequence, float duration = 1.0, bool autokill = true);

	/**
	 * Adds the given activity as gesture.
	 *
	 * Note: This internally calls for CBaseAnimating::SelectWeightedSequence
	 * in order to retrieve the animation sequence index.
	 *
	 * @param activity	Activity to set.
	 * @param duration	Duration of the gesture.
	 * @param autokill	Whether or not, the gesture should be removed upon animation completion.
	 * @return		Layer index.
	 * @error 		Invalid entity.
	 */
	public native int AddGesture(Activity activity, float duration = 1.0, bool autokill = true);

	/**
	 * Is the given activity playing as gesture?
	 *
	 * @param activity	Activity to check.
	 * @return		True if playing, False otherwise.
	 * @error 		Invalid entity.
	 */
	public native bool IsPlayingGesture(Activity activity);

	/**
	 * Restarts the given gesture activity.
	 *
	 * @param activity	Activity to restart.
	 * @param addifmissing	Adds the given activity if it's missing as gesture.
	 * @param autokill	Whether or not, the gesture should be removed upon animation completion.
	 * @error 		Invalid entity.
	 */
	public native void RestartGesture(Activity activity, bool addifmissing = true, bool autokill = true);

	/**
	 * Removes all the gestures.
	 *
	 * @error 		Invalid entity.
	 */
	public native void RemoveAllGestures();

	/**
	 * Adds the given sequence as gesture.
	 *
	 * @param sequence	Animation sequence index.
	 * @param priority	The priority of the gesture.
	 * @return		Layer index.
	 * @error 		Invalid entity.
	 */
	public native int AddLayeredSequence(int sequence, int priority);

	/**
	 * Sets the priority on the given layer.
	 *
	 * @param layer		Layer index.
	 * @param priority	The priority of the gesture.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerPriority(int layer, int priority);

	/**
	 * Is the layer index valid?
	 *
	 * @param layer		Layer index.
	 * @return		True if it's valid, false otherwise.
	 * @error 		Invalid entity.
	 */
	public native bool IsValidLayer(int layer);

	/**
	 * Sets the duration of a gesture.
	 *
	 * @param layer		Layer index.
	 * @param duration	Gesture duration.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerDuration(int layer, float duration);

	/**
	 * Gets the duration of a gesture.
	 *
	 * @param layer		Layer index.
	 * @return		Layer duration, 0.0 if layer index is invalid.
	 * @error 		Invalid entity.
	 */
	public native float GetLayerDuration(int layer);

	/**
	 * Sets the cycle progress of given gesture.
	 *
	 * @param layer		Layer index.
	 * @param cycle		Cycle progress to set.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerCycle(int layer, float cycle);

	/**
	 * Obtains the cycle progress of given gesture.
	 *
	 * @param layer		Layer index.
	 * @return		Cycle progress, 0.0 if the layer index is invalid.
	 * @error 		Invalid entity.
	 */
	public native float GetLayerCycle(int layer);

	/**
	 * Sets the playback rate of given gesture.
	 *
	 * @param layer			Layer index.
	 * @param playerbackRate	Playback rate to set.
	 * @error 			Invalid entity.
	 */
	public native void SetLayerPlaybackRate(int layer, float playbackRate);

	/**
	 * Sets the weight of given gesture.
	 *
	 * @param layer		Layer index.
	 * @param weight	Weight to set.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerWeight(int layer, float weight);

	/**
	 * Gets the weight of given gesture.
	 *
	 * @param layer		Layer index.
	 * @return		Layer's weight, 0.0 if layer index is invalid.
	 * @error 		Invalid entity.
	 */
	public native float GetLayerWeight(int layer);

	/**
	 * Sets the blend-in of given gesture.
	 *
	 * @param layer		Layer index.
	 * @param blendIn	Blend-in value to set.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerBlendIn(int iLayer, float blendIn);

	/**
	 * Sets the blend-out of given gesture.
	 *
	 * @param layer		Layer index.
	 * @param blendOut	Blend-out value to set.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerBlendOut(int layer, float blendOut);

	/**
	 * Sets whether or not the given gesture should be deleted,
	 * upon animation completion.
	 *
	 * @param layer		Layer index.
	 * @param autokill	True for deletion, False otherwise.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerAutokill(int layer, bool autokill);

	/**
	 * Sets whether or not the given gesture should loop,
	 * upon animation completion.
	 *
	 * @param layer		Layer index.
	 * @param autokill	True for looping, False otherwise.
	 * @error 		Invalid entity.
	 */
	public native void SetLayerLooping(int layer, bool looping);

	/* */
	public native void SetLayerNoRestore(int layer, bool noRestore);

	/**
	 * Gets the given gesture's activity.
	 *
	 * @param layer		Layer index.
	 * @return		Gesture's activity.
	 * @error 		Invalid entity.
	 */
	public native Activity GetLayerActivity(int layer);

	/**
	 * Gets the given gesture's sequence.
	 *
	 * @param layer		Layer index.
	 * @return		Gesture's sequence.
	 * @error 		Invalid entity.
	 */
	public native int GetLayerSequence(int layer);

	/**
	 * Finds a layer index for the given activity.
	 *
	 * @param activity	Activity to look for.
	 * @return		Layer index if there's a gesture with this activity, -1 otherwise.
	 * @error 		Invalid entity.
	 */
	public native int FindGestureLayer(Activity activity);

	/* */
	public native void RemoveLayer(int layer, float killRate = 0.2, float killDelay = 0.0);

	/**
	 * Removes the given gesture immediately.
	 *
	 * @param layer		Layer index.
	 * @error 		Invalid entity.
	 */
	public native void FastRemoveLayer(int layer);

	/**
	 * Obtains the given gesture interface.
	 *
	 * Note: Do not store animation layer in arrays. This is a memory address.
	 *
	 * @return		Memory address of the layer.
	 * @error 		Invalid entity.
	 */
	public native CAnimationLayer GetAnimOverlay(int layer);

	/**
	 * Gets the number of gestures.
	 *
	 * @return		Number of gestures.
	 * @error 		Invalid entity.
	 */
	public native int GetNumAnimOverlays();

	/**
	 * Sets the number of gestures.
	 *
	 * @param num		Number of gestures.
	 * @error 		Invalid entity.
	 */
	public native void SetNumAnimOverlays(int num);

	/**
	 * Is it playing a gesture?
	 *
	 * @return		True if playing a gesture, False otherwise.
	 * @error 		Invalid entity.
	 */
	public native bool HasActiveLayer();
	
	// To-Do?: Move this to extension
	public int FindGestureLayerBySequence(int iSequence)
	{
		// i = layer index
		int iCount = this.GetNumAnimOverlays();
		for(int i = 0; i < iCount; i++) 
		{
			CAnimationLayer pOverlay = this.GetAnimOverlay(i); 
			
			// Validate layer
			if (!pOverlay.IsAlive()) 
			{
				continue; 
			}

			// Validate sequence
			if (pOverlay.m_nSequence == iSequence) 
			{
				return i; 
			}
		}

		// Return on the unsuccess
		return -1; 
	}

};