#include "helpers.h"

int nothing;

IPluginFunction *GetFunctionByNameEx(IPluginContext *pContext, const char *name)
{
	IPluginRuntime *pRuntime = pContext->GetRuntime();
	for(uint32_t i = 0; i < pRuntime->GetPublicsNum(); i++)
	{
		sp_public_t *pub = nullptr;
		if(pRuntime->GetPublicByIndex(i, &pub) == SP_ERROR_NONE)
		{
			if(strstr(pub->name, name) != nullptr)
			{
				return pRuntime->GetFunctionById(pub->funcid);
			}
		}
	}
	return nullptr;
}

void PawnVectorToVector(cell_t *vecAddr, Vector &vector) {
	vector.x = sp_ctof(vecAddr[0]);
	vector.y = sp_ctof(vecAddr[1]);
	vector.z = sp_ctof(vecAddr[2]);
}

void PawnVectorToVector(cell_t *angAddr, QAngle &angle) {
	angle.x = sp_ctof(angAddr[0]);
	angle.y = sp_ctof(angAddr[1]);
	angle.z = sp_ctof(angAddr[2]);
}

void PawnVectorToVector(cell_t *vecAddr, Vector *vector) {
	vector->x = sp_ctof(vecAddr[0]);
	vector->y = sp_ctof(vecAddr[1]);
	vector->z = sp_ctof(vecAddr[2]);
}

void PawnVectorToVector(cell_t *angAddr, QAngle *angle) {
	angle->x = sp_ctof(angAddr[0]);
	angle->y = sp_ctof(angAddr[1]);
	angle->z = sp_ctof(angAddr[2]);
}

void VectorToPawnVector(cell_t *vecAddr, const Vector vector) {
	vecAddr[0] = sp_ftoc(vector.x);
	vecAddr[1] = sp_ftoc(vector.y);
	vecAddr[2] = sp_ftoc(vector.z);
}

void VectorToPawnVector(cell_t *angAddr, const QAngle angle) {
	angAddr[0] = sp_ftoc(angle.x);
	angAddr[1] = sp_ftoc(angle.y);
	angAddr[2] = sp_ftoc(angle.z);
}

void VectorToPawnVector(cell_t *vecAddr, const Vector *vector) {
	vecAddr[0] = sp_ftoc(vector->x);
	vecAddr[1] = sp_ftoc(vector->y);
	vecAddr[2] = sp_ftoc(vector->z);
}

void VectorToPawnVector(cell_t *angAddr, const QAngle *angle) {
	angAddr[0] = sp_ftoc(angle->x);
	angAddr[1] = sp_ftoc(angle->y);
	angAddr[2] = sp_ftoc(angle->z);
}

SendProp *GetEntSendProp(CBaseEntity *pEntity, const char *prop)
{
	IServerUnknown *pUnk = (IServerUnknown *)pEntity;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	ServerClass *pClass = pNet->GetServerClass();
	
	sm_sendprop_info_t sendprop;
	gamehelpers->FindSendPropInfo(pClass->GetName(), prop, &sendprop);
	
	return sendprop.prop;
}

int GetEntPropOffset(CBaseEntity *pEntity, const char *prop, bool local, int &fieldsize, int &fieldsizeinbytes)
{
	IServerUnknown *pUnk = (IServerUnknown *)pEntity;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	ServerClass *pClass = pNet->GetServerClass();
	
	sm_sendprop_info_t sendprop;
	gamehelpers->FindSendPropInfo(pClass->GetName(), prop, &sendprop);
	
	datamap_t *datamap = gamehelpers->GetDataMap(pEntity);
	sm_datatable_info_t dataprop;
	gamehelpers->FindDataMapInfo(datamap, prop, &dataprop);
	typedescription_t *td = dataprop.prop;
	
	fieldsizeinbytes = td->fieldSizeInBytes;
	fieldsize = td->fieldSize;
	
	int sendoffset = sendprop.actual_offset;
	int local_sendoffset = sendprop.prop->GetOffset();
	
	int dataoffset = dataprop.actual_offset;

	#if SOURCE_ENGINE == SE_TF2
	int local_dataoffset = td->fieldOffset[TD_OFFSET_NORMAL];
	#endif

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
	int local_dataoffset = td->fieldOffset;
	#endif
	
	if(local)
	{
		if(local_sendoffset > 0 && local_dataoffset <= 0)
		{
			return local_sendoffset;
		}
		else if(local_sendoffset <= 0 && local_dataoffset > 0)
		{
			return local_dataoffset;
		}
		else if(local_sendoffset > 0 && local_dataoffset > 0)
		{
			return local_sendoffset;
		}
		
		return -1;
	}
	else
	{
		if(sendoffset > 0 && dataoffset <= 0)
		{
			return sendoffset;
		}
		else if(sendoffset <= 0 && dataoffset > 0)
		{
			return dataoffset;
		}
		else if(sendoffset > 0 && dataoffset > 0)
		{
			return sendoffset;
		}
		
		return -1;
	}
}

const char *HandleErrorToString(HandleError err)
{
	switch(err)
	{
		case HandleError_None: { return "No error"; }
		case HandleError_Changed: { return "The handle has been freed and reassigned"; }
		case HandleError_Type: { return "The handle has a different type registered"; }
		case HandleError_Freed: { return "The handle has been freed"; }
		case HandleError_Index: { return "generic internal indexing error"; }
		case HandleError_Access: { return "No access permitted to free this handle"; }
		case HandleError_Limit: { return "The limited number of handles has been reached"; }
		case HandleError_Identity: { return "The identity token was not usable"; }
		case HandleError_Owner: { return "Owners do not match for this operation"; }
		case HandleError_Version: { return "Unrecognized security structure version"; }
		case HandleError_Parameter: { return "An invalid parameter was passed"; }
		case HandleError_NoInherit: { return "This type cannot be inherited"; }
	}

	return "";
}