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
#include "MSelectPlayers.h"

#define ADD_GADGET(gd, box, up, dn, lf, rt, txt) \
  gd.mg_boxOnScreen = box; \
  gd.mg_pmgUp = up; \
  gd.mg_pmgDown = dn; \
  gd.mg_pmgLeft = lf; \
  gd.mg_pmgRight = rt; \
  gd.SetText(txt); \
  AddChild(&gd);

extern CTString astrNoYes[2];
extern CTString astrSplitScreenRadioTexts[4];
extern void SelectPlayersFillMenu(void);
extern void SelectPlayersApplyMenu(void);

void CSelectPlayersMenu::Initialize_t(void) {
  // intialize split screen menu
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(LOCALIZE("SELECT PLAYERS"));
  AddChild(&gm_mgTitle);

  TRIGGER_MG(gm_mgDedicated, 0, gm_mgStart, gm_mgObserver, LOCALIZE("Dedicated:"), astrNoYes);
  gm_mgDedicated.mg_strTip = LOCALIZE("select to start dedicated server");
  gm_mgDedicated.mg_pOnTriggerChange = NULL;

  TRIGGER_MG(gm_mgObserver, 1, gm_mgDedicated, gm_mgSplitScreenCfg, LOCALIZE("Observer:"), astrNoYes);
  gm_mgObserver.mg_strTip = LOCALIZE("select to join in for observing, not for playing");
  gm_mgObserver.mg_pOnTriggerChange = NULL;

  // split screen config trigger
  TRIGGER_MG(gm_mgSplitScreenCfg, 2, gm_mgObserver, gm_mgPlayer0Change, LOCALIZE("Number of players:"), astrSplitScreenRadioTexts);
  gm_mgSplitScreenCfg.mg_strTip = LOCALIZE("choose more than one player to play in split screen");
  gm_mgSplitScreenCfg.mg_pOnTriggerChange = NULL;

  gm_mgPlayer0Change.mg_iCenterI = -1;
  gm_mgPlayer1Change.mg_iCenterI = -1;
  gm_mgPlayer2Change.mg_iCenterI = -1;
  gm_mgPlayer3Change.mg_iCenterI = -1;
  gm_mgPlayer0Change.mg_boxOnScreen = BoxMediumMiddle(4);
  gm_mgPlayer1Change.mg_boxOnScreen = BoxMediumMiddle(5);
  gm_mgPlayer2Change.mg_boxOnScreen = BoxMediumMiddle(6);
  gm_mgPlayer3Change.mg_boxOnScreen = BoxMediumMiddle(7);
  gm_mgPlayer0Change.mg_strTip =
    gm_mgPlayer1Change.mg_strTip =
    gm_mgPlayer2Change.mg_strTip =
    gm_mgPlayer3Change.mg_strTip = LOCALIZE("select profile for this player");
  AddChild(&gm_mgPlayer0Change);
  AddChild(&gm_mgPlayer1Change);
  AddChild(&gm_mgPlayer2Change);
  AddChild(&gm_mgPlayer3Change);

  gm_mgNotes.mg_boxOnScreen = BoxMediumRow(9.0);
  gm_mgNotes.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgNotes.mg_iCenterI = -1;
  gm_mgNotes.mg_bEnabled = FALSE;
  gm_mgNotes.mg_bLabel = TRUE;
  AddChild(&gm_mgNotes);
  gm_mgNotes.SetText("");

  /*  // options button
  mgSplitOptions.SetText(LOCALIZE("Game options");
  mgSplitOptions.mg_boxOnScreen = BoxMediumRow(3);
  mgSplitOptions.mg_bfsFontSize = BFS_MEDIUM;
  mgSplitOptions.mg_iCenterI = 0;
  mgSplitOptions.mg_pmgUp = &mgSplitLevel;
  mgSplitOptions.mg_pmgDown = &mgSplitStartStart;
  mgSplitOptions.mg_strTip = LOCALIZE("adjust game rules");
  mgSplitOptions.mg_pActivatedFunction = &StartGameOptionsFromSplitScreen;
  AddChild(& mgSplitOptions.mg_lnNode);*/

  /*  // start button
  mgSplitStartStart.mg_bfsFontSize = BFS_LARGE;
  mgSplitStartStart.mg_boxOnScreen = BoxBigRow(4);
  mgSplitStartStart.mg_pmgUp = &mgSplitOptions;
  mgSplitStartStart.mg_pmgDown = &mgSplitGameType;
  mgSplitStartStart.SetText(LOCALIZE("START");
  AddChild(&mgSplitStartStart);
  mgSplitStartStart.mg_pActivatedFunction = &StartSelectPlayersMenuFromSplit;
  */

  ADD_GADGET(gm_mgStart, BoxBigRow(7), &gm_mgSplitScreenCfg, &gm_mgPlayer0Change, NULL, NULL, LOCALIZE("START"));
  gm_mgStart.mg_bfsFontSize = BFS_LARGE;
  gm_mgStart.mg_iCenterI = 0;
}

void CSelectPlayersMenu::StartMenu(void) {
  CGameMenu::StartMenu();
  SelectPlayersFillMenu();
  SelectPlayersApplyMenu();
}

void CSelectPlayersMenu::EndMenu(void) {
  SelectPlayersApplyMenu();
  CGameMenu::EndMenu();
}