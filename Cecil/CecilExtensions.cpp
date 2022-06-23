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

#include "Cecil/CecilExtensions.h"

// Render game in the background while in menu
INDEX sam_bBackgroundGameRender = TRUE;

// FOV patch
INDEX sam_bUseVerticalFOV = TRUE;
FLOAT sam_fCustomFOV = -1.0f;

// Red screen on damage
INDEX sam_bRedScreenOnDamage = TRUE;

// Custom command registy
static CDynamicContainer<CShellSymbol> _cCustomSymbols;

#define CUSTOM_SYMBOLS_CONFIG "Scripts\\CustomSymbols.ini"

// Register a certain command after its change
static void CECIL_RegisterCommand(void *pCommand) {
  CShellSymbol *pss = NULL;

  for (INDEX i = 0; i < _pShell->sh_assSymbols.Count(); i++) {
    CShellSymbol *pssCheck = _pShell->sh_assSymbols.Pointer(i);

    // Matching command
    if (pssCheck->ss_pvValue == pCommand) {
      pss = pssCheck;
      break;
    }
  }

  if (pss == NULL) {
    CPrintF("Could not find corresponding command in the shell for registering!\n");
    return;
  }

  // Add symbols to the list
  if (!_cCustomSymbols.IsMember(pss)) {
    _cCustomSymbols.Add(pss);
  }

  // Save symbol values
  try {
    CTFileStream strm;
    strm.Create_t(CTFILENAME(CUSTOM_SYMBOLS_CONFIG));

    FOREACHINDYNAMICCONTAINER(_cCustomSymbols, CShellSymbol, itss) {
      CShellSymbol *pssWrite = itss;

      // Symbol name
      strm.FPrintF_t("%s = ", pssWrite->ss_strName);

      // Symbol value
      strm.FPrintF_t("%s;\n", _pShell->GetValue(pssWrite->ss_strName));
    }

    strm.Close();

  } catch (char *strError) {
    CPrintF("Could not save custom symbols: %s\n", strError);
  }
};

// Custom initialization
void CECIL_Init(void) {
  // Function patches
  extern void CECIL_InitPatches(void);
  CECIL_InitPatches();

  {
    CPrintF("Intercepting Engine functions:\n");

    extern void CECIL_ApplyFOVPatch(void);
    extern void CECIL_ApplyScreenBlendPatch(void);
    CECIL_ApplyFOVPatch();
    CECIL_ApplyScreenBlendPatch();

    CPrintF("  done!\n");
  }

  // Command registry
  _pShell->DeclareSymbol("void CECIL_RegisterCommand(INDEX);", &CECIL_RegisterCommand);

  // Custom symbols
  {
    // Render game in the background while in menu
    _pShell->DeclareSymbol("user INDEX sam_bBackgroundGameRender post:CECIL_RegisterCommand;", &sam_bBackgroundGameRender);

    // FOV patch
    _pShell->DeclareSymbol("user INDEX sam_bUseVerticalFOV post:CECIL_RegisterCommand;", &sam_bUseVerticalFOV);
    _pShell->DeclareSymbol("user FLOAT sam_fCustomFOV      post:CECIL_RegisterCommand;", &sam_fCustomFOV);

    // Red screen on damage
    _pShell->DeclareSymbol("user INDEX sam_bRedScreenOnDamage post:CECIL_RegisterCommand;", &sam_bRedScreenOnDamage);
  }

  // Restore custom symbol values
  _pShell->Execute("include \"" CUSTOM_SYMBOLS_CONFIG "\";");
};
