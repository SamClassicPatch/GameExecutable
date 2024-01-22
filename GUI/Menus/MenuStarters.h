/* Copyright (c) 2002-2012 Croteam Ltd.
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

#ifndef SE_INCL_MENU_STARTERS_H
#define SE_INCL_MENU_STARTERS_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

void StartVideoOptionsMenu(void);
void StartAudioOptionsMenu(void);
void StartNetworkMenu(void);
void StartNetworkJoinMenu(void);
void StartNetworkStartMenu(void);
void StartNetworkOpenMenu(void);
void StartSplitScreenMenu(void);
void StartSplitStartMenu(void);
void StartSinglePlayerNewMenuCustom(void);
void StartSinglePlayerNewMenu(void);
void StartSinglePlayerQuickLoadMenu(void);
void StartSinglePlayerLoadMenu(void);
void StartSinglePlayerSaveMenu(void);
void StartDemoLoadMenu(void);
void StartDemoSaveMenu(void);
void StartNetworkQuickLoadMenu(void);
void StartNetworkLoadMenu(void);
void StartNetworkSaveMenu(void);
void StartSplitScreenQuickLoadMenu(void);
void StartSplitScreenLoadMenu(void);
void StartSplitScreenSaveMenu(void);
// [Cecil] Extra arguments for customization
void StartVarGameOptions(const CTString &strTitle, const CTFileName &fnmConfig, CGameMenu *pgmParentMenu);
void StartSinglePlayerGameOptions(void);
void StartGameOptionsFromNetwork(void);
void StartPatchServerOptionsFromNetwork(void); // [Cecil]
void StartGameOptionsFromSplitScreen(void);
void StartRenderingOptionsMenu(void);
void StartCustomizeKeyboardMenu(void);
void StartCustomizeCommonControlsMenu(void); // [Cecil]
void StartCustomizeAxisMenu(void);
void StartOptionsMenu(void);
void StartCurrentLoadMenu();
void StartCurrentSaveMenu();
void StartCurrentQuickLoadMenu();
void StartChangePlayerMenuFromOptions(void);
void StartChangePlayerMenuFromSinglePlayer(void);
void StartControlsMenuFromPlayer(void);
void StartControlsMenuFromOptions(void);
void StartSelectLevelFromSingle(void);
void StartHighScoreMenu(void);
void StartSelectPlayersMenuFromSplit(void);
void StartSelectPlayersMenuFromNetwork(void);
void StartSelectPlayersMenuFromOpen(void);
void StartSelectPlayersMenuFromServers(void);
void StartSelectServerLAN(void);
void StartSelectServerNET(void);
void StartSelectLevelFromSplit(void);
void StartSelectLevelFromNetwork(void);
void StartSelectPlayersMenuFromSplitScreen(void);
void StartSelectPlayersMenuFromNetworkLoad(void);
void StartSelectPlayersMenuFromSplitScreenLoad(void);
void StartPlayerModelLoadMenu(void);
void StartControlsLoadMenu(void);
void StartCustomLoadMenu(void);
void StartAddonsLoadMenu(void);
void StartModsLoadMenu(void);
void StartExtrasMenu(void); // [Cecil]
void StartPatchCreditsMenu(void); // [Cecil]
void StartNetworkSettingsMenu(void);
void StartPatchOptionsMenu(void); // [Cecil]
void StartSinglePlayerMenu(void);
void DisabledFunction(void);

#endif /* include-once check. */