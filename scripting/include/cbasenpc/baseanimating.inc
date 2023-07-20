#if defined _CBASENPC_BASEANIMATING_INC_
 #endinput
#endif
#define _CBASENPC_BASEANIMATING_INC_

#include "activity.inc"

methodmap CBaseAnimating < CBaseEntity
{
	public CBaseAnimating(int entity) 
	{
		return view_as<CBaseAnimating>(entity);
	}
	
	/**
	 * This is the vtable offset of CBaseAnimating::HandleAnimEvent.
	 *
	 * @return       VTable offset.
	 */
	/*property int OffsetHandleAnimEvent {
		public native get();
	}*/
	public static native int iHandleAnimEvent();
	
#if defined _dhooks_included
	/**
	 * Hooks CBaseAnimating::HandleAnimEvent.
	 *
	 * @param callback 	DHookcallback that will be used.
	 * @error		Invalid entity.
	 */
	public void Hook_HandleAnimEvent(DHookCallback callback)
	{
		static DynamicHook hHook = null;
		if (hHook == null)
		{
			hHook = new DynamicHook(CBaseAnimating.iHandleAnimEvent(), HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity);
			if (hHook == null) return;
			hHook.AddParam(HookParamType_ObjectPtr);
		}
		hHook.HookEntity(Hook_Pre, this.index, callback);
	}
#endif
	
	/**
	 * Tells the animation system to advance animation on entity.
	 *
	 * Note: CBaseNPC's IBody component calls this function automatically.
	 * However any entities that inherit from CBaseAnimating, must call this function
	 * every server frame. Otherwise they will not play any animations!
	 * 
	 * @error 		Invalid entity.
	 */
	public native void StudioFrameAdvance();

	/**
	 * Progresses through the potential events the currently playing animation has.
	 * And triggers CBaseAnimating::HandleAnimEvent.
	 * 
	 * @param eventHandler	Entity to fire CBaseAnimating::HandleAnimEvent function, with the animation events from your entity.
	 * @error 		Invalid entity.
	 */
	public native void DispatchAnimEvents(CBaseAnimating anim);

	/**
	 * Retrieves the sequence index of a named animation. (NOT ACTIVITY!)
	 * 
	 * @param name		Name of the animation.
	 * @return		Animation sequence index on success, -1 otherwise.
	 * @error 		Invalid entity.
	 */
	public native int LookupSequence(const char[] name);

	/**
	 * Retrieves the duration of a given animation sequence.
	 * 
	 * @param sequence	Animation sequence index.
	 * @return		Sequence duration in seconds.
	 * @error 		Invalid entity.
	 */
	public native float SequenceDuration(int sequence);

	/**
	 * Retrieves a random animation sequence, stored under a given activity.
	 * 
	 * @param activity	The activity under which the animation sequences are stored.
	 * @return		Animation sequence index on success, -1 otherwise.
	 * @error 		Invalid entity.
	 */
	public native int SelectWeightedSequence(Activity activity);

	/**
	 * Resets the animation sequence.
	 * 
	 * @param sequence	Animation sequence index.
	 * @error 		Invalid entity.
	 */
	public native void ResetSequence(int sequence);

	/**
	 * Retrieves the index of a named attachment.
	 * 
	 * @param attachmentName	Attachment index.
	 * @error 			Invalid entity.
	 */
	public native int LookupAttachment(const char[] attachmentName);

	/**
	 * Retrieves the world position & angles of a given attachment.
	 * 
	 * @param attachmentIndex	Attachment index.
	 * @param vecOrigin		Destination vector, to store the position in.
	 * @param vecAngles		Destination vector, to store the angles in.
	 * @return			True on success, False otherwise.
	 * @error 			Invalid entity.
	 */
	public native bool GetAttachment(int attachmentIndex, float vecOrigin[3], float vecAngles[3]);

	/**
	 * Retrieves the world matrix of a given attachment.
	 * 
	 * @param attachmentIndex	Attachment index.
	 * @param matrix		Destination mattrix, to store attachment in.
	 * @return			True on success, False otherwise.
	 * @error 			Invalid entity.
	 */
	public native bool GetAttachmentMatrix(int attachmentIndex, float matrix[3][4]);
	
	/**
	 * Obtains the entity's CStudioHdr ptr.
	 *
	 * @return		Memory address of CStudioHdr.
	 * @error 		Invalid entity.
	 */
	public native Address GetModelPtr();

	/**
	 * Retrieves the parameter index of a named pose parameter.
	 *
	 * @param name		Name of the pose parameter.
	 * @return		Pose parameter index on success, -1 otherwise.
	 * @error 		Invalid entity.
	 */
	public native int LookupPoseParameter(const char[] name);

	/**
	 * Sets the pose parameter to the given value.
	 *
	 * @param poseParam	Pose parameter index.
	 * @param newValue	Value to set.
	 * @return		Value set for the pose parameter.
	 * @error 		Invalid entity.
	 */
	public native float SetPoseParameter(int poseParam, float newValue);

	/**
	 * Obtains the pose parameter value.
	 *
	 * @param poseParam	Pose parameter index.
	 * @return		Value set for the pose parameter.
	 * @error 		Invalid entity.
	 */
	public native float GetPoseParameter(int poseParam);
};