#if defined _CBASENPC_BMS_NAV_INC_
 #endinput
#endif
#define _CBASENPC_BMS_NAV_INC_

#include "../nav.inc"

enum BlackMesaNavAttributeType
{
    BM_NAV_NONE = 0,
    BM_NAV_PLAYER_BASE = (1 << 0),
};

methodmap CBlackMesaNavArea < CNavArea
{
    /** Returns Black Mesa-specific game attributes. */
    public native BlackMesaNavAttributeType GetGameAttributes();

    /** Adds one or more Black Mesa-specific game attributes. */
    public native void SetGameAttribute(BlackMesaNavAttributeType attributes);

    /** Removes one or more Black Mesa-specific game attributes. */
    public native void ClearGameAttribute(BlackMesaNavAttributeType attributes);

    /** Returns true when at least one requested game attribute is present. */
    public native bool HasGameAttribute(BlackMesaNavAttributeType attributes);
};
