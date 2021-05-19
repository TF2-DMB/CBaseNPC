/**
 * vim: set ts=4 :
 * =============================================================================
 * CBASENPC Extension
 * Copyright (C) 2017-2021  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * Version: $Id$
 */
#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_

/**
 * @file smsdk_config.h
 * @brief Contains macros for configuring basic extension information.
 */
 
/* Basic information exposed publicly */
#define SMEXT_CONF_NAME			"CBaseNPC"
#define SMEXT_CONF_DESCRIPTION	"Primarily exposes valve's NextBot framework. But also various functions from public SDK."
#define SMEXT_CONF_VERSION		"1.5"
#define SMEXT_CONF_AUTHOR		"Kenzzer & Arthurdead & Pelipoika"
#define SMEXT_CONF_URL			""
#define SMEXT_CONF_LOGTAG		"CBASENPC"
#define SMEXT_CONF_LICENSE		"GPL"
#define SMEXT_CONF_DATESTRING	__DATE__

/** 
 * @brief Exposes plugin's main interface.
 */
#define SMEXT_LINK(name) SDKExtension *g_pExtensionIface = name;

/**
 * @brief Sets whether or not this plugin required Metamod.
 * NOTE: Uncomment to enable, comment to disable.
 */
#define SMEXT_CONF_METAMOD		

/** Enable interfaces you want to use here by uncommenting lines */
#define SMEXT_ENABLE_FORWARDSYS
#define SMEXT_ENABLE_HANDLESYS
#define SMEXT_ENABLE_PLAYERHELPERS
//#define SMEXT_ENABLE_DBMANAGER
#define SMEXT_ENABLE_GAMECONF
//#define SMEXT_ENABLE_MEMUTILS
#define SMEXT_ENABLE_GAMEHELPERS
//#define SMEXT_ENABLE_TIMERSYS
//#define SMEXT_ENABLE_THREADER
//#define SMEXT_ENABLE_LIBSYS
//#define SMEXT_ENABLE_MENUS
//#define SMEXT_ENABLE_ADTFACTORY
#define SMEXT_ENABLE_PLUGINSYS
//#define SMEXT_ENABLE_ADMINSYS
//#define SMEXT_ENABLE_TEXTPARSERS
//#define SMEXT_ENABLE_USERMSGS
//#define SMEXT_ENABLE_TRANSLATOR
//#define SMEXT_ENABLE_ROOTCONSOLEMENU

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
