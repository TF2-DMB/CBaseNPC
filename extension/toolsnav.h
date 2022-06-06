#include "sourcesdk/nav.h"
#include "sourcesdk/basedoor.h"
#include "sourcesdk/funcbrush.h"

extern ConVar* nav_solid_props;

inline bool IsEntityWalkable(CBaseEntity* entityParam, unsigned int flags)
{
	CBaseEntityHack* entity = static_cast<CBaseEntityHack*>(entityParam);

	if (FClassnameIs(entity, "worldspawn"))
	{
		return false;
	}

	if (FClassnameIs(entity, "player"))
	{
		return false;
	}

	// if we hit a door, assume its walkable because it will open when we touch it
	if (FClassnameIs(entity, "func_door*"))
	{
		if (!entity->HasSpawnFlags(SF_DOOR_PTOUCH))
		{
			// this door is not opened by touching it, if it is closed, the area is blocked
			CBaseDoorHack* door = (CBaseDoorHack*)entity;
			return door->GetToggleState() == TS_AT_TOP;
		}
		return (flags & WALK_THRU_FUNC_DOORS) ? true : false;
	}

	if (FClassnameIs(entity, "prop_door*"))
	{
		return (flags & WALK_THRU_PROP_DOORS) ? true : false;
	}

	// if we hit a clip brush, ignore it if it is not BRUSHSOLID_ALWAYS
	if (FClassnameIs(entity, "func_brush"))
	{
		CFuncBrushHack *brush = (CFuncBrushHack *)entity;
		switch (brush->GetSolidity())
		{
		case CFuncBrushHack::BRUSHSOLID_ALWAYS:
			return false;
		case CFuncBrushHack::BRUSHSOLID_NEVER:
			return true;
		case CFuncBrushHack::BRUSHSOLID_TOGGLE:
			return (flags & WALK_THRU_TOGGLE_BRUSHES) ? true : false;
		}
	}

	// if we hit a breakable object, assume its walkable because we will shoot it when we touch it
	if (FClassnameIs(entity, "func_breakable") && entity->GetHealth() && entity->GetTakeDamage() == DAMAGE_YES)
	{
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	}

	if (FClassnameIs(entity, "func_breakable_surf") && entity->GetTakeDamage() == DAMAGE_YES)
	{
		return (flags & WALK_THRU_BREAKABLES) ? true : false;
	}

	if (FClassnameIs(entity, "func_playerinfected_clip") == true)
	{
		return true;
	}

	if (nav_solid_props->GetBool() && FClassnameIs(entity, "prop_*"))
	{
		return true;
	}
	return false;
}