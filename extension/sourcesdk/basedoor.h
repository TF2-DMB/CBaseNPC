#pragma once

#include "sourcesdk/basetoggle.h"

#define SF_DOOR_ROTATE_YAW			0
#define	SF_DOOR_START_OPEN_OBSOLETE	(1 << 0)
#define SF_DOOR_ROTATE_BACKWARDS	(1 << 1)
#define SF_DOOR_NONSOLID_TO_PLAYER	(1 << 2)
#define SF_DOOR_PASSABLE			(1 << 3)
#define SF_DOOR_ONEWAY				(1 << 4)
#define	SF_DOOR_NO_AUTO_RETURN		(1 << 5)
#define SF_DOOR_ROTATE_ROLL			(1 << 6)
#define SF_DOOR_ROTATE_PITCH		(1 << 7)
#define SF_DOOR_PUSE				(1 << 8)
#define SF_DOOR_NONPCS				(1 << 9)
#define SF_DOOR_PTOUCH				(1 << 10)
#define SF_DOOR_LOCKED				(1 << 11)
#define SF_DOOR_SILENT				(1 << 12)
#define	SF_DOOR_USE_CLOSES			(1 << 13)
#define SF_DOOR_SILENT_TO_NPCS		(1 << 14)
#define SF_DOOR_IGNORE_USE			(1 << 15)
#define SF_DOOR_NEW_USE_RULES		(1 << 16)

class CBaseDoor : public CBaseToggle
{
};