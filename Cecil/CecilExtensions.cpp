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

// Red screen on damage
INDEX sam_bRedScreenOnDamage = TRUE;

// Custom initialization
void CECIL_Init(void) {
  // Initialize the core
  CECIL_InitCore();

  {
    CPrintF("--- Sam: Intercepting Engine functions ---\n");

    extern void CECIL_ApplyScreenBlendPatch(void);
    extern void CECIL_ApplyUndecoratedPatch(void);
    extern void CECIL_ApplySoundListenPatch(void);
    CECIL_ApplyScreenBlendPatch();
    CECIL_ApplyUndecoratedPatch();
    CECIL_ApplySoundListenPatch();

    CPrintF("--- Done! ---\n");
  }

  // Custom symbols
  _pShell->DeclareSymbol("persistent user INDEX sam_bBackgroundGameRender;", &sam_bBackgroundGameRender);
  _pShell->DeclareSymbol("persistent user INDEX sam_bOptionTabs;",           &sam_bOptionTabs);
};
