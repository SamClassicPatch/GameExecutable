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
#include "MenuStuff.h"
#include "MSinglePlayer.h"

void CSinglePlayerMenu::Initialize_t(void) {
  // intialize single player menu
  gm_mgTitle.SetName(LOCALIZE("SINGLE PLAYER"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  gm_mgPlayerLabel.mg_boxOnScreen = BoxBigRow(-1.0f);
  gm_mgPlayerLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgPlayerLabel.mg_iCenterI = -1;
  gm_mgPlayerLabel.mg_bEnabled = FALSE;
  gm_mgPlayerLabel.mg_bLabel = TRUE;
  AddChild(&gm_mgPlayerLabel);

  gm_mgNewGame.SetText(LOCALIZE("NEW GAME"));
  gm_mgNewGame.mg_bfsFontSize = BFS_LARGE;
  gm_mgNewGame.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgNewGame.mg_strTip = LOCALIZE("start new game with current player");
  AddChild(&gm_mgNewGame);
  gm_mgNewGame.mg_pmgUp = &gm_mgOptions;
  gm_mgNewGame.mg_pmgDown = &gm_mgCustom;
  gm_mgNewGame.mg_pActivatedFunction = NULL;

  gm_mgCustom.SetText(LOCALIZE("CUSTOM LEVEL"));
  gm_mgCustom.mg_bfsFontSize = BFS_LARGE;
  gm_mgCustom.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgCustom.mg_strTip = LOCALIZE("start new game on a custom level");
  AddChild(&gm_mgCustom);
  gm_mgCustom.mg_pmgUp = &gm_mgNewGame;
  gm_mgCustom.mg_pmgDown = &gm_mgQuickLoad;
  gm_mgCustom.mg_pActivatedFunction = NULL;

  gm_mgQuickLoad.SetText(LOCALIZE("QUICK LOAD"));
  gm_mgQuickLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuickLoad.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgQuickLoad.mg_strTip = LOCALIZE("load a quick-saved game (F9)");
  AddChild(&gm_mgQuickLoad);
  gm_mgQuickLoad.mg_pmgUp = &gm_mgCustom;
  gm_mgQuickLoad.mg_pmgDown = &gm_mgLoad;
  gm_mgQuickLoad.mg_pActivatedFunction = NULL;

  gm_mgLoad.SetText(LOCALIZE("LOAD"));
  gm_mgLoad.mg_bfsFontSize = BFS_LARGE;
  gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgLoad.mg_strTip = LOCALIZE("load a saved game of current player");
  AddChild(&gm_mgLoad);
  gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
  gm_mgLoad.mg_pmgDown = &gm_mgTraining;
  gm_mgLoad.mg_pActivatedFunction = NULL;

  gm_mgTraining.SetText(LOCALIZE("TRAINING"));
  gm_mgTraining.mg_bfsFontSize = BFS_LARGE;
  gm_mgTraining.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgTraining.mg_strTip = LOCALIZE("start training level - KarnakDemo");
  AddChild(&gm_mgTraining);
  gm_mgTraining.mg_pmgUp = &gm_mgLoad;
  gm_mgTraining.mg_pmgDown = &gm_mgTechTest;
  gm_mgTraining.mg_pActivatedFunction = NULL;

  gm_mgTechTest.SetText(LOCALIZE("TECHNOLOGY TEST"));
  gm_mgTechTest.mg_bfsFontSize = BFS_LARGE;
  gm_mgTechTest.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgTechTest.mg_strTip = LOCALIZE("start technology testing level");
  AddChild(&gm_mgTechTest);
  gm_mgTechTest.mg_pmgUp = &gm_mgTraining;
  gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;
  gm_mgTechTest.mg_pActivatedFunction = NULL;

  gm_mgPlayersAndControls.mg_bfsFontSize = BFS_LARGE;
  gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgPlayersAndControls.mg_pmgUp = &gm_mgTechTest;
  gm_mgPlayersAndControls.mg_pmgDown = &gm_mgOptions;
  gm_mgPlayersAndControls.SetText(LOCALIZE("PLAYERS AND CONTROLS"));
  gm_mgPlayersAndControls.mg_strTip = LOCALIZE("change currently active player or adjust controls");
  AddChild(&gm_mgPlayersAndControls);
  gm_mgPlayersAndControls.mg_pActivatedFunction = NULL;

  gm_mgOptions.SetText(LOCALIZE("GAME OPTIONS"));
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgOptions.mg_strTip = LOCALIZE("adjust miscellaneous game options");
  AddChild(&gm_mgOptions);
  gm_mgOptions.mg_pmgUp = &gm_mgPlayersAndControls;
  gm_mgOptions.mg_pmgDown = &gm_mgNewGame;
  gm_mgOptions.mg_pActivatedFunction = NULL;
}

void CSinglePlayerMenu::StartMenu(void) {
  gm_mgTraining.mg_bEnabled = GetGameAPI()->IsMenuEnabledSS("Training");
  gm_mgTechTest.mg_bEnabled = GetGameAPI()->IsMenuEnabledSS("Technology Test");

  if (gm_mgTraining.mg_bEnabled) {
    AddChild(&gm_mgTraining);

    gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
    gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
    gm_mgLoad.mg_pmgDown = &gm_mgTraining;

    gm_mgTraining.mg_boxOnScreen = BoxBigRow(4.0f);
    gm_mgTraining.mg_pmgUp = &gm_mgLoad;
    gm_mgTraining.mg_pmgDown = &gm_mgTechTest;

    gm_mgTechTest.mg_boxOnScreen = BoxBigRow(5.0f);
    gm_mgTechTest.mg_pmgUp = &gm_mgTraining;
    gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;

    gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(6.0f);
    gm_mgOptions.mg_boxOnScreen = BoxBigRow(7.0f);

  } else {
    // [Cecil] Unparent training button
    gm_mgTraining.SetParent(NULL);

    gm_mgLoad.mg_boxOnScreen = BoxBigRow(3.0f);
    gm_mgLoad.mg_pmgUp = &gm_mgQuickLoad;
    gm_mgLoad.mg_pmgDown = &gm_mgTechTest;

    gm_mgTechTest.mg_boxOnScreen = BoxBigRow(4.0f);
    gm_mgTechTest.mg_pmgUp = &gm_mgLoad;
    gm_mgTechTest.mg_pmgDown = &gm_mgPlayersAndControls;

    gm_mgPlayersAndControls.mg_boxOnScreen = BoxBigRow(5.0f);
    gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  }

  CGameMenu::StartMenu();

  CPlayerCharacter &pc = *GetGameAPI()->GetPlayerCharacter(GetGameAPI()->GetPlayerForSP());

  CTString strPlayer;
  strPlayer.PrintF(LOCALIZE("Player: %s\n"), pc.GetNameForPrinting());

  gm_mgPlayerLabel.SetText(strPlayer);
}
