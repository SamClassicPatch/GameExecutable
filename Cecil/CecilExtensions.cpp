/* Copyright (c) 2022-2023 Dreamy Cecil
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

#include "CmdLine.h"
#include "LevelInfo.h"

#include <CoreLib/Interfaces/DataFunctions.h>

// General
INDEX sam_bPatchVersionLabel = TRUE;
INDEX sam_bBackgroundGameRender = TRUE;
INDEX sam_bOptionTabs = TRUE;
INDEX sam_bLevelCategories = TRUE;
INDEX sam_bDecoratedServerNames = TRUE;

INDEX sam_bNotifyAboutUpdates = TRUE;
INDEX sam_iUpdateReminder = 0;

// Start some level immediately (like on '+level' argument)
void StartMap(const CTString &strLevel) {
  // Add levels directory and replace the slashes
  CTFileName fnmStart = "Levels\\" + strLevel;
  IData::ReplaceChar(fnmStart.str_String, '/', '\\');

  // Add world extension
  if (fnmStart.FileExt() != ".wld") {
    fnmStart += ".wld";
  }

  // Original command line function code
  CPrintF(LOCALIZE("Command line world: '%s'\n"), fnmStart.str_String);

  try {
    // Set marker to teleport to
    if (cmd_iGoToMarker >= 0) {
      CPrintF(LOCALIZE("Command line marker: %d\n"), cmd_iGoToMarker);
      _pShell->Execute(CTString(0, "cht_iGoToMarker = %d;", cmd_iGoToMarker));
    }

    GetGameAPI()->SetCustomLevel(fnmStart);

    // Start multiplayer game
    if (cmd_bServer) {
      extern void StartNetworkGame(void);
      StartNetworkGame();

    } else {
      extern void StartSinglePlayerGame(void);
      StartSinglePlayerGame();
    }

  } catch (char *strError) {
    CPrintF(LOCALIZE("Cannot start '%s': '%s'\n"), fnmStart.str_String, strError);
  }
};

// Custom initialization
void CECIL_Init(void) {
  // Initialize the core
  CECIL_InitCore();

#if CLASSICSPATCH_ENGINEPATCHES

  // Function patches
  CPutString("--- Sam: Intercepting Engine functions ---\n");
  _EnginePatches.CorePatches();
  CPutString("--- Done! ---\n");

#endif // CLASSICSPATCH_ENGINEPATCHES

  // Custom symbols
  _pShell->DeclareSymbol("persistent user INDEX sam_bPatchVersionLabel;",    &sam_bPatchVersionLabel);
  _pShell->DeclareSymbol("persistent user INDEX sam_bBackgroundGameRender;", &sam_bBackgroundGameRender);
  _pShell->DeclareSymbol("persistent user INDEX sam_bOptionTabs;",           &sam_bOptionTabs);
  _pShell->DeclareSymbol("persistent user INDEX sam_bLevelCategories;",      &sam_bLevelCategories);
  _pShell->DeclareSymbol("persistent user INDEX sam_bDecoratedServerNames;", &sam_bDecoratedServerNames);

  _pShell->DeclareSymbol("persistent user INDEX sam_bNotifyAboutUpdates;", &sam_bNotifyAboutUpdates);
  _pShell->DeclareSymbol("persistent      INDEX sam_iUpdateReminder;",     &sam_iUpdateReminder);

  _pShell->DeclareSymbol("user void ListLevels(CTString);", &ListLevels);

  // Commands for starting maps
  _pShell->DeclareSymbol("user INDEX cmd_iGoToMarker;", &cmd_iGoToMarker);
  _pShell->DeclareSymbol("user INDEX cmd_bServer;", &cmd_bServer);
  _pShell->DeclareSymbol("user void StartMap(CTString);", &StartMap);
};
