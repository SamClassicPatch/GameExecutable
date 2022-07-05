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

#include "PatchAPI.h"

// Define external patch API
CPatchAPI *_pPatchAPI = NULL;

// List available function patches
static void ListFuncPatches(void) {
  if (_pPatchAPI->aPatches.Count() == 0) {
    CPrintF("No function patches available!\n");
    return;
  }

  CPrintF("Available function patches:\n");
  
  for (INDEX iPatch = 0; iPatch < _pPatchAPI->aPatches.Count(); iPatch++) {
    CPrintF(" %d - %s\n", iPatch, _pPatchAPI->aPatches[iPatch].strName);
  }
};

// Enable specific function patch
static void EnableFuncPatch(INDEX iPatch) {
  iPatch = Clamp(iPatch, (INDEX)0, INDEX(_pPatchAPI->aPatches.Count() - 1));

  SFuncPatch &fpPatch = _pPatchAPI->aPatches[iPatch];
  fpPatch.pPatch->set_patch();

  if (fpPatch.pPatch->ok()) {
    CPrintF("Successfully set '%s' function patch!\n", fpPatch.strName);
  } else {
    CPrintF("Cannot set '%s' function patch!\n", fpPatch.strName);
  }
};

// Disable specific function patch
static void DisableFuncPatch(INDEX iPatch) {
  iPatch = Clamp(iPatch, (INDEX)0, INDEX(_pPatchAPI->aPatches.Count() - 1));
  
  SFuncPatch &fpPatch = _pPatchAPI->aPatches[iPatch];
  fpPatch.pPatch->remove_patch();

  CPrintF("Successfully removed '%s' function patch!\n", fpPatch.strName);
};

// Constructor
CPatchAPI::CPatchAPI() {
  // Add patch API to symbols
  CShellSymbol &ssNew = *_pShell->sh_assSymbols.New(1);

  ssNew.ss_strName = "PatchAPI"; // Access by this symbol name
  ssNew.ss_istType = 0; // Should be '_shell_istUndeclared'
  ssNew.ss_pvValue = this; // Pointer to self
  ssNew.ss_ulFlags = SSF_CONSTANT; // Unchangable
  ssNew.ss_pPreFunc = NULL; // Unused
  ssNew.ss_pPostFunc = NULL; // Unused

  strVersion = "1.1.2";

  // Session properties game modes used by the patch
  sp_aiGameModes.New(2);
  sp_aiGameModes[0] = -1; // Flyover
  sp_aiGameModes[1] =  0; // Cooperative
  
  // Session properties difficulties used by the patch
  sp_aGameDifficulties.New(6);
  sp_aGameDifficulties[0] = Difficulty(-1, "Tourist");
  sp_aGameDifficulties[1] = Difficulty( 0, "Easy");
  sp_aGameDifficulties[2] = Difficulty( 1, "Normal");
  sp_aGameDifficulties[3] = Difficulty( 2, "Hard");
  sp_aGameDifficulties[4] = Difficulty( 3, "Serious");
  sp_aGameDifficulties[5] = Difficulty( 4, "Mental");

  // Output patcher actions
  if (FileExists(_fnmApplicationExe.FileDir() + "PatcherOutput")) {
    Patch_DebugOutput() = true;
  }

  // Commands for manually toggling function patches
  _pShell->DeclareSymbol("void ListPatches(void);",   &ListFuncPatches);
  _pShell->DeclareSymbol("void EnablePatch(INDEX);",  &EnableFuncPatch);
  _pShell->DeclareSymbol("void DisablePatch(INDEX);", &DisableFuncPatch);
};
