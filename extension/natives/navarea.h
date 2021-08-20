#ifndef NATIVES_AREA_H
#define NATIVES_AREA_H

#include "sourcesdk/nav_area.h"

#if SOURCE_ENGINE == SE_TF2
#include "sourcesdk/tf_nav_area.h"
#endif

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

#define NAVAREA_NATIVE(name) \
	cell_t CNavArea_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CNavArea *pArea = (CNavArea *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		} \

#define NAVLADDER_NATIVE(name) \
	cell_t CNavLadder_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CNavLadder *pArea = (CNavLadder *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		} \
	
	

NAVAREA_NATIVE(UpdateBlocked)
	pArea->UpdateBlocked(params[2], params[3]);
	return 1;
}

NAVAREA_NATIVE(IsBlocked)
	return pArea->IsBlocked(params[2], params[3]);
}

NAVAREA_NATIVE(GetID)
	return pArea->GetID();
}

NAVAREA_NATIVE(SetParent)
	pArea->SetParent((CNavArea *)params[2], (NavTraverseType)params[3]);
return 1;
}

NAVAREA_NATIVE(GetParent)
	return (cell_t)pArea->GetParent();
}

NAVAREA_NATIVE(GetParentHow)
	return pArea->GetParentHow();
}

NAVAREA_NATIVE(SetCostSoFar)
	pArea->SetCostSoFar(sp_ctof(params[2]));
	return 1;
}

NAVAREA_NATIVE(GetCostSoFar)
	return sp_ftoc(pArea->GetCostSoFar());
}

NAVAREA_NATIVE(GetAttributes)
	return pArea->GetAttributes();
}

NAVAREA_NATIVE(GetCenter)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pArea->GetCenter();
	VectorToPawnVector(posAddr, pos);
	return 1;
}

NAVAREA_NATIVE(IsConnected)
	return pArea->IsConnected((CNavArea *)params[2], (NavDirType)params[3]);
}

NAVAREA_NATIVE(IsEdge)
	return pArea->IsEdge((NavDirType)params[2]);
}

NAVAREA_NATIVE(Contains)
	return pArea->Contains((CNavArea *)params[2]);
}

NAVAREA_NATIVE(GetSizeX)
	return sp_ftoc(pArea->GetSizeX());
}

NAVAREA_NATIVE(GetSizeY)
	return sp_ftoc(pArea->GetSizeY());
}

NAVAREA_NATIVE(GetZ)
	return sp_ftoc(pArea->GetZ(sp_ctof(params[2]), sp_ctof(params[3])));
}

NAVAREA_NATIVE(GetZVector)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	return sp_ftoc(pArea->GetZ(dst));
}

NAVAREA_NATIVE(ComputeNormal)
	cell_t *norAddr;
	pContext->LocalToPhysAddr(params[2], &norAddr);
	Vector nor;
	pArea->ComputeNormal(&nor, params[3]);
	VectorToPawnVector(norAddr, nor);
	return 1;
}

NAVLADDER_NATIVE(lengthGet)
	return sp_ftoc(pArea->m_length);
}

#if SOURCE_ENGINE == SE_TF2
#define TFNAVAREA_NATIVE(name) \
	cell_t CTFNavArea_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CTFNavArea *pArea = (CTFNavArea *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		}

TFNAVAREA_NATIVE(GetAttributesTF)
	return pArea->GetAttributesTF();
}

TFNAVAREA_NATIVE(SetAttributeTF)
	pArea->SetAttributeTF(params[2]);
	return 0;
}

TFNAVAREA_NATIVE(ClearAttributeTF)
	pArea->ClearAttributeTF(params[2]);
	return 0;
}

TFNAVAREA_NATIVE(HasAttributeTF)
	return pArea->HasAttributeTF(params[2]);
}
#endif

#endif