/**
 * vim: set ts=4 :
 * =============================================================================
 * Tickrate Enabler
 * Copyright (C) 2012 Michael "ProdigySim" Busby
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
 *
 * As a special exception, the author(s) give you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, the author(s) grant
 * this exception to all derivative works.  The author(s) define further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */
#include <cstdlib>
#include "memutils.h"
#include "convar_sm_l4d.h"
#include "tier0/icommandline.h"

#include "sourcehook.h"
#include "sourcehook_impl.h"
#include "sourcehook_impl_chookidman.h"

#include "codepatch/patchmanager.h"

#include "tickrate_enabler.h"
#include "patchexceptions.h"

#include "boomervomitpatch.h"
#include "maxrate_patches.h"
#include "updaterate_patches.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef _DEBUG
#define VDEBUG "-dev"
#else
#define VDEBUG ""
#endif
#define VERSION_L4D "1.5"
#define VERSION_L4D2 "1.5"
#if defined (_L4D)
#define VERSION VERSION_L4D VDEBUG
#elif defined (_L4D2)
#define VERSION VERSION_L4D2 VDEBUG
#endif

//
L4DTickRate g_L4DTickRatePlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(L4DTickRate, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_L4DTickRatePlugin);

L4DTickRate::L4DTickRate() {}

SourceHook::Impl::CSourceHookImpl g_SourceHook;
SourceHook::ISourceHook *g_SHPtr = &g_SourceHook;
int g_PLID = 0;

SH_DECL_HOOK0(IServerGameDLL, GetTickInterval, const, 0, float);

float GetTickInterval()
{
	float tickinterval = (1.0f / 30.0f);

	if ( CommandLine()->CheckParm( "-tickrate" ) )
	{
		float tickrate = CommandLine()->ParmValue( "-tickrate", 0 );
		Msg("Tickrate_Enabler: Read TickRate %f\n", tickrate);
		if ( tickrate > 10 )
			tickinterval = 1.0f / tickrate;
	}

	RETURN_META_VALUE(MRES_SUPERCEDE, tickinterval );
}

IServerGameDLL *gamedll = NULL;
IVEngineServer *engine = NULL;
ICvar * g_pCvar = NULL;

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool L4DTickRate::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	gamedll = (IServerGameDLL *)gameServerFactory(INTERFACEVERSION_SERVERGAMEDLL, NULL);
	if(!gamedll)
	{
		Error("Tickrate_Enabler: Failed to get a pointer on ServerGameDLL.\n");
		return false;
	}

	engine = (IVEngineServer *)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);

	SH_ADD_HOOK(IServerGameDLL, GetTickInterval, gamedll, SH_STATIC(GetTickInterval), false);

	try
	{
		m_patchManager.Register(new BoomerVomitFrameTimePatch(gamedll));
		m_patchManager.Register(new NetChanDataRatePatch((BYTE *)engine));
#if defined (CGAMECLIENT_PATCH)
		m_patchManager.Register(new GameClientSetRatePatch((BYTE *)engine));
#endif
#if defined (CLAMPCLIENTRATE_PATCH)
		m_patchManager.Register(new ClampClientRatePatch((BYTE *)engine));
#endif
		m_patchManager.Register(new CGameClientUpdateRatePatch((BYTE *)engine));
		m_patchManager.Register(new CBaseClientUpdateRatePatch((BYTE *)engine));
		
		m_patchManager.PatchAll();
	}
	catch( PatchException & e)
	{
		Error("!!!!!\nPatch Failure: %s\n!!!!!\n", e.GetDescription());
		Error("Failed to process all tickrate_enabler patches, bailing out.\n");
		return false;
	}

	g_pCvar = reinterpret_cast<ICvar *>(interfaceFactory(CVAR_INTERFACE_VERSION,NULL));
	if(g_pCvar == NULL)
	{
		Error("RecordingHelpers: Failed to get Cvar interface.\n");
		return false;
	}

	g_pCvar->FindVar("sv_maxrate")->SetMax(false,0.0);
	g_pCvar->FindVar("sv_minrate")->SetMax(false,0.0);
	g_pCvar->FindVar("net_splitpacket_maxrate")->SetMax(false,0.0);
	// Maybe later
	//g_pCvar->FindVar("tv_maxrate")->SetMax(false, 0.0); 
	

	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void L4DTickRate::Unload( void )
{
	g_pCvar->FindVar("sv_maxrate")->SetMax(true,30000.0);
	g_pCvar->FindVar("sv_minrate")->SetMax(true,30000.0);
	g_pCvar->FindVar("net_splitpacket_maxrate")->SetMax(true,30000.0);
	m_patchManager.UnpatchAll();
	m_patchManager.UnregisterAll();
	SH_REMOVE_HOOK(IServerGameDLL, GetTickInterval, gamedll, SH_STATIC(GetTickInterval), false);
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *L4DTickRate::GetPluginDescription( void )
{
	return "Tickrate_Enabler " VERSION ", ProdigySim";
}
