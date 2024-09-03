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
#ifndef __UPDATERATE_PATCHES_H__
#define __UPDATERATE_PATCHES_H__

#include "codepatch/icodepatch.h"
#include "misc_asm.h"

class CGameClientUpdateRatePatch : public ICodePatch
{
public:
	CGameClientUpdateRatePatch(BYTE * engine);
	~CGameClientUpdateRatePatch();
	void Patch();
	void Unpatch();
private:
	BYTE * FindCGameClientSetUpdateRate(BYTE * engine);
	ICodePatch * GeneratePatch(BYTE * pCGameClientUpdateRate);
	ICodePatch * m_patch;
};

class CBaseClientUpdateRatePatch : public ICodePatch
{
public:
	CBaseClientUpdateRatePatch(BYTE * engine);
	~CBaseClientUpdateRatePatch();
	void Patch();
	void Unpatch();
private:
	BYTE * FindCBaseClientSetUpdateRate(BYTE * engine);
	ICodePatch * GeneratePatch(BYTE * pCBaseClientUpdateRate);
	ICodePatch * m_patch;
};

#endif