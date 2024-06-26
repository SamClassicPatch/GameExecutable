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

#include "StdH.h"
#include "MenuPrinting.h"
#include "MInGame.h"

void CInGameMenu::Initialize_t(void) {
  // intialize main menu
  gm_mgTitle.SetName(LOCALIZE("GAME"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  gm_mgLabel1.SetText("");
  gm_mgLabel1.mg_boxOnScreen = BoxMediumRow(-2.0);
  gm_mgLabel1.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgLabel1.mg_iCenterI = -1;
  gm_mgLabel1.mg_bEnabled = FALSE;
  gm_mgLabel1.mg_bLabel = TRUE;
  AddChild(&gm_mgLabel1);

  gm_mgLabel2.SetText("");
  gm_mgLabel2.mg_boxOnScreen = BoxMediumRow(-1.0);
  gm_mgLabel2.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgLabel2.mg_iCenterI = -1;
  gm_mgLabel2.mg_bEnabled = FALSE;
  gm_mgLabel2.mg_bLabel = TRUE;
  AddChild(&gm_mgLabel2);

  gm_mgQuickLoad.SetText(LOCALIZE("QUICK LOAD"));
  gm_mgQuickLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickLoad.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgQuickLoad.mg_strTip = LOCALIZE("load a quick-saved game (F9)");
  AddChild(&gm_mgQuickLoad);
  gm_mgQuickLoad.mg_pmgUp = &gm_mgQuit;
  gm_mgQuickLoad.mg_pmgDown = &gm_mgQuickSave;
  gm_mgQuickLoad.mg_pActivatedFunction = NULL;

  gm_mgQuickSave.SetText(LOCALIZE("QUICK SAVE"));
  gm_mgQuickSave.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickSave.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgQuickSave.mg_strTip = LOCALIZE("quick-save current game (F6)");
  AddChild(&gm_mgQuickSave);
  gm_mgQuickSave.mg_pmgUp = &gm_mgQuickLoad;
  gm_mgQuickSave.mg_pmgDown = &gm_mgLoad;
  gm_mgQuickSave.mg_pActivatedFunction = NULL;

  gm_mgLoad.SetText(LOCALIZE("LOAD"));
  gm_mgLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgLoad.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgLoad.mg_strTip = LOCALIZE("load a saved game");
  AddChild(&gm_mgLoad);
  gm_mgLoad.mg_pmgUp = &gm_mgQuickSave;
  gm_mgLoad.mg_pmgDown = &gm_mgSave;
  gm_mgLoad.mg_pActivatedFunction = NULL;

  gm_mgSave.SetText(LOCALIZE("SAVE"));
  gm_mgSave.mg_bfsFontSize = BFS_LARGE;
  gm_mgSave.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgSave.mg_strTip = LOCALIZE("save current game (each player has own slots!)");
  AddChild(&gm_mgSave);
  gm_mgSave.mg_pmgUp = &gm_mgLoad;
  gm_mgSave.mg_pmgDown = &gm_mgDemoRec;
  gm_mgSave.mg_pActivatedFunction = NULL;

  gm_mgDemoRec.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgDemoRec.mg_bfsFontSize = BFS_LARGE;
  gm_mgDemoRec.mg_pmgUp = &gm_mgSave;
  gm_mgDemoRec.mg_pmgDown = &gm_mgHighScore;
  gm_mgDemoRec.SetText("Text not set");
  AddChild(&gm_mgDemoRec);
  gm_mgDemoRec.mg_pActivatedFunction = NULL;

  gm_mgHighScore.SetText(LOCALIZE("HIGH SCORES"));
  gm_mgHighScore.mg_bfsFontSize = BFS_LARGE;
  gm_mgHighScore.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgHighScore.mg_strTip = LOCALIZE("view list of top ten best scores");
  AddChild(&gm_mgHighScore);
  gm_mgHighScore.mg_pmgUp = &gm_mgDemoRec;
  gm_mgHighScore.mg_pmgDown = &gm_mgOptions;
  gm_mgHighScore.mg_pActivatedFunction = NULL;

  gm_mgOptions.SetText(LOCALIZE("OPTIONS"));
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgOptions.mg_strTip = LOCALIZE("adjust video, audio and input options");
  AddChild(&gm_mgOptions);
  gm_mgOptions.mg_pmgUp = &gm_mgHighScore;
  gm_mgOptions.mg_pmgDown = &gm_mgStop;
  gm_mgOptions.mg_pActivatedFunction = NULL;

  gm_mgStop.SetText(LOCALIZE("STOP GAME"));
  gm_mgStop.mg_bfsFontSize = BFS_LARGE;
  gm_mgStop.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgStop.mg_strTip = LOCALIZE("stop currently running game");
  AddChild(&gm_mgStop);
  gm_mgStop.mg_pmgUp = &gm_mgOptions;
  gm_mgStop.mg_pmgDown = &gm_mgQuit;
  gm_mgStop.mg_pActivatedFunction = NULL;

  gm_mgQuit.SetText(LOCALIZE("QUIT"));
  gm_mgQuit.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuit.mg_boxOnScreen = BoxBigRow(8.0f);
  gm_mgQuit.mg_strTip = LOCALIZE("exit game immediately");
  AddChild(&gm_mgQuit);
  gm_mgQuit.mg_pmgUp = &gm_mgStop;
  gm_mgQuit.mg_pmgDown = &gm_mgQuickLoad;
  gm_mgQuit.mg_pActivatedFunction = NULL;
}

void CInGameMenu::StartMenu(void) {
  gm_mgQuickLoad.mg_bEnabled = _pNetwork->IsServer();
  gm_mgQuickSave.mg_bEnabled = _pNetwork->IsServer();
  gm_mgLoad.mg_bEnabled = _pNetwork->IsServer();
  gm_mgSave.mg_bEnabled = _pNetwork->IsServer();
  gm_mgDemoRec.mg_bEnabled = TRUE; //_pNetwork->IsServer();
  extern void SetDemoStartStopRecText();
  SetDemoStartStopRecText();

  // [Cecil] Check if player name should be displayed on the pause menu
  BOOL bPlayerName = (_gmRunningGameMode == GM_SINGLE_PLAYER);
#if SE1_GAME == SS_REV
  bPlayerName |= _pNetwork->IsServer();
#endif

  if (bPlayerName) {
    CPlayerCharacter &pc = *GetGameAPI()->GetPlayerCharacter(GetGameAPI()->GetProfileForSP());

    CTString strPlayer;
    strPlayer.PrintF(LOCALIZE("Player: %s"), pc.GetNameForPrinting());

    gm_mgLabel1.SetText(strPlayer);
    gm_mgLabel2.SetText("");

#if SE1_GAME != SS_REV
  } else if (_pNetwork->IsServer()) {
    CTString strHost, strAddress;
    CTString strHostName;
    _pNetwork->GetHostName(strHost, strAddress);
    if (strHost == "") {
      strHostName = LOCALIZE("<not started yet>");
    } else {
      strHostName = strHost + " (" + strAddress + ")";
    }

    gm_mgLabel1.SetText(LOCALIZE("Address: ") + strHostName);
    gm_mgLabel2.SetText("");
#endif

  } else {
    CTString strConfig;
    strConfig = LOCALIZE("<not adjusted>");
    extern CTString sam_strNetworkSettings;
    if (sam_strNetworkSettings != "") {
      LoadStringVar(CTFileName(sam_strNetworkSettings).NoExt() + ".des", strConfig);
      strConfig.OnlyFirstLine();
    }

    gm_mgLabel1.SetText(LOCALIZE("Connected to: ") + GetGameAPI()->JoinAddress());
    gm_mgLabel2.SetText(LOCALIZE("Connection: ") + strConfig);
  }

  CGameMenu::StartMenu();
}