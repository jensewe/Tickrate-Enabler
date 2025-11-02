/**
 * vim: set ts=4 :
 * =============================================================================
 * MaxRate Patches
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


#include "updaterate_patches.h"
#include "thirdparty/memutils.h"
#include "patchexceptions.h"
#include "basicbinpatch.h"
#include "sigs.h"

CGameClientUpdateRatePatch::CGameClientUpdateRatePatch(BYTE * engine) : m_patch(NULL)
{
	m_patch = GeneratePatch(FindCGameClientSetUpdateRate(engine));
}

CGameClientUpdateRatePatch::~CGameClientUpdateRatePatch()
{
	if(m_patch != NULL) delete m_patch;
}

void CGameClientUpdateRatePatch::Patch()
{
	m_patch->Patch();
}

void CGameClientUpdateRatePatch::Unpatch()
{
	m_patch->Unpatch();
}

BYTE * CGameClientUpdateRatePatch::FindCGameClientSetUpdateRate(BYTE * engine)
{
#if defined (_LINUX)
	return (BYTE *)g_MemUtils.SimpleResolve(engine, SIG_CGAMECLIENT_SETUPDATERATE);
#elif defined (_WIN32)
	return (BYTE*)g_MemUtils.FindLibPattern(engine, SIG_CGAMECLIENT_SETUPDATERATE, SIG_CGAMECLIENT_SETUPDATERATE_LEN);
#endif
}

ICodePatch * CGameClientUpdateRatePatch::GeneratePatch(BYTE * pCGameClientUpdateRate)
{
	if(!pCGameClientUpdateRate)
	{
		throw PatchException("Unable to find CGameClient::SetUpdateRate!");
	}
	if(pCGameClientUpdateRate[CGAMECLIENT_SETUPDATERATE_PATCH_OFFSET] != CGAMECLIENT_SETUPDATERATE_PATCH_CHECK_BYTE)
	{
		throw PatchException("CGameClient::SetUpdateRate patch offset incorrect!");
	}
#if defined _WIN32
	const BYTE replacement[] = {JMP_8_OPCODE};
	return new BasicStaticBinPatch<sizeof(replacement)>(pCGameClientUpdateRate+CGAMECLIENT_SETUPDATERATE_PATCH_OFFSET, replacement);
#elif defined _LINUX
	return new BasicStaticBinPatch<sizeof(NOP_2)>(pCGameClientUpdateRate+CGAMECLIENT_SETUPDATERATE_PATCH_OFFSET, NOP_2);
#endif
}

CBaseClientUpdateRatePatch::CBaseClientUpdateRatePatch(BYTE * engine) : m_patch(NULL)
{
	m_patch = GeneratePatch(FindCBaseClientSetUpdateRate(engine));
}

CBaseClientUpdateRatePatch::~CBaseClientUpdateRatePatch()
{
	if(m_patch != NULL) delete m_patch;
}

void CBaseClientUpdateRatePatch::Patch()
{
	m_patch->Patch();
}

void CBaseClientUpdateRatePatch::Unpatch()
{
	m_patch->Unpatch();
}

BYTE * CBaseClientUpdateRatePatch::FindCBaseClientSetUpdateRate(BYTE * engine)
{
#if defined (_LINUX)
	return (BYTE *)g_MemUtils.SimpleResolve(engine, SIG_CBASECLIENT_SETUPDATERATE);
#elif defined (_WIN32)
	return (BYTE*)g_MemUtils.FindLibPattern(engine, SIG_CBASECLIENT_SETUPDATERATE, SIG_CBASECLIENT_SETUPDATERATE_LEN);
#endif
}

ICodePatch * CBaseClientUpdateRatePatch::GeneratePatch(BYTE * pCBaseClientUpdateRate)
{
	if(!pCBaseClientUpdateRate)
	{
		throw PatchException("Unable to find CBaseClient::SetUpdateRate!");
	}
	if(pCBaseClientUpdateRate[CBASECLIENT_SETUPDATERATE_PATCH_OFFSET] != CBASECLIENT_SETUPDATERATE_PATCH_CHECK_BYTE)
	{
		throw PatchException("CBaseClient::SetUpdateRate patch offset incorrect!");
	}
#if defined _WIN32
	const BYTE replacement[] = {JMP_8_OPCODE};
	return new BasicStaticBinPatch<sizeof(replacement)>(pCBaseClientUpdateRate+CBASECLIENT_SETUPDATERATE_PATCH_OFFSET, replacement);
#elif defined _LINUX
	const BYTE replacement[] = {0x89, 0xC2, NOP_1[0]};
	return new BasicStaticBinPatch<sizeof(replacement)>(pCBaseClientUpdateRate+CBASECLIENT_SETUPDATERATE_PATCH_OFFSET-1, replacement);
#endif
}
