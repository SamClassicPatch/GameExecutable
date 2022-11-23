/* Copyright (c) 2022 Dreamy Cecil
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

// General
INDEX sam_bBackgroundGameRender = TRUE;
INDEX sam_bOptionTabs = TRUE;

// Custom initialization
void CECIL_Init(void) {
  // Initialize the core
  CECIL_InitCore();

  // Function patches
  CPutString("--- Sam: Intercepting Engine functions ---\n");
  {
    _EnginePatches.CorePatches();
  }
  CPutString("--- Done! ---\n");

  // Render patch commands
  _pShell->DeclareSymbol("persistent user INDEX sam_bAdjustForAspectRatio;", &_EnginePatches._bAdjustForAspectRatio);
  _pShell->DeclareSymbol("persistent user INDEX sam_bUseVerticalFOV;", &_EnginePatches._bUseVerticalFOV);
  _pShell->DeclareSymbol("persistent user FLOAT sam_fCustomFOV;",      &_EnginePatches._fCustomFOV);
  _pShell->DeclareSymbol("persistent user FLOAT sam_fThirdPersonFOV;", &_EnginePatches._fThirdPersonFOV);
  _pShell->DeclareSymbol("           user INDEX sam_bCheckFOV;",       &_EnginePatches._bCheckFOV);

  // Custom symbols
  _pShell->DeclareSymbol("persistent user INDEX sam_bBackgroundGameRender;", &sam_bBackgroundGameRender);
  _pShell->DeclareSymbol("persistent user INDEX sam_bOptionTabs;",           &sam_bOptionTabs);
};
