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
#include "MCustomizeKeyboard.h"

void CCustomizeKeyboardMenu::FillListItems(void) {
  // disable all items first
  for (INDEX i = 0; i < KEYS_ON_SCREEN; i++) {
    gm_mgKey[i].mg_bEnabled = FALSE;
    gm_mgKey[i].mg_iInList = -2;
  }

  // [Cecil] Current actions
  CListHead &lhActions = GetGameAPI()->GetActions(gm_pControls);

  BOOL bHasFirst = FALSE;
  BOOL bHasLast = FALSE;
  // set diks to key buttons
  INDEX iLabel = 0;
  INDEX ctLabels = lhActions.Count();
  FOREACHINLIST(CButtonAction, ba_lnNode, lhActions, itAct) {
    INDEX iInMenu = iLabel - gm_iListOffset;
    if ((iLabel >= gm_iListOffset) && (iLabel < (gm_iListOffset + gm_ctListVisible))) {
      bHasFirst |= (iLabel == 0);
      bHasLast |= (iLabel == ctLabels - 1);
      gm_mgKey[iInMenu].SetName(TRANSV(itAct->ba_strName));
      gm_mgKey[iInMenu].mg_iControlNumber = iLabel;
      gm_mgKey[iInMenu].SetBindingNames(FALSE);
      gm_mgKey[iInMenu].mg_strTip = LOCALIZE("Enter - change binding, Backspace - unbind");
      gm_mgKey[iInMenu].mg_bEnabled = TRUE;
      gm_mgKey[iInMenu].mg_iInList = iLabel;
    }
    iLabel++;
  }

  // enable/disable up/down arrows
  gm_mgArrowUp.mg_bEnabled = !bHasFirst && ctLabels > 0;
  gm_mgArrowDn.mg_bEnabled = !bHasLast && ctLabels > 0;
}

void CCustomizeKeyboardMenu::Initialize_t(void) {
  // intialize Audio options menu
  gm_mgTitle.SetName(LOCALIZE("CUSTOMIZE BUTTONS"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

#define KL_START 3.0f
#define KL_STEEP -1.45f
  for (INDEX iLabel = 0; iLabel < KEYS_ON_SCREEN; iLabel++) {
    INDEX iPrev = (gm_ctListVisible + iLabel - 1) % KEYS_ON_SCREEN;
    INDEX iNext = (iLabel + 1) % KEYS_ON_SCREEN;
    // initialize label entities
    gm_mgKey[iLabel].mg_boxOnScreen = BoxKeyRow(iLabel - 1.0f);
    // initialize label gadgets
    gm_mgKey[iLabel].mg_pmgUp = &gm_mgKey[iPrev];
    gm_mgKey[iLabel].mg_pmgDown = &gm_mgKey[iNext];
    gm_mgKey[iLabel].mg_bVisible = TRUE;
    AddChild(&gm_mgKey[iLabel]);
  }
  // arrows just exist
  AddChild(&gm_mgArrowDn);
  AddChild(&gm_mgArrowUp);
  gm_mgArrowDn.mg_adDirection = AD_DOWN;
  gm_mgArrowUp.mg_adDirection = AD_UP;
  gm_mgArrowDn.mg_boxOnScreen = BoxArrow(AD_DOWN);
  gm_mgArrowUp.mg_boxOnScreen = BoxArrow(AD_UP);
  gm_mgArrowDn.mg_pmgRight = gm_mgArrowDn.mg_pmgUp = &gm_mgKey[KEYS_ON_SCREEN - 1];
  gm_mgArrowUp.mg_pmgRight = gm_mgArrowUp.mg_pmgDown = &gm_mgKey[0];

  gm_ctListVisible = KEYS_ON_SCREEN;
  gm_pmgArrowUp = &gm_mgArrowUp;
  gm_pmgArrowDn = &gm_mgArrowDn;
  gm_pmgListTop = &gm_mgKey[0];
  gm_pmgListBottom = &gm_mgKey[KEYS_ON_SCREEN - 1];

  // [Cecil] No controls yet
  gm_pControls = NULL;
}

// [Cecil] Set current controls for the menu and the keys
void CCustomizeKeyboardMenu::SetControls(CControls *pctrl) {
  gm_pControls = pctrl;

  // Pass controls to key gadgets
  for (INDEX i = 0; i < KEYS_ON_SCREEN; i++) {
    gm_mgKey[i].mg_pControls = pctrl;
  }
};

void CCustomizeKeyboardMenu::StartMenu(void) {
  ControlsMenuOn();
  gm_iListOffset = 0;
  gm_ctListTotal = GetGameAPI()->GetActions(gm_pControls).Count(); // [Cecil] Current actions
  gm_iListWantedItem = 0;
  CGameMenu::StartMenu();
}

void CCustomizeKeyboardMenu::EndMenu(void) {
  ControlsMenuOff();

  // [Cecil] Reload common controls
  if (gm_pControls == GetGameAPI()->pctrlCommon) {
    try {
      gm_pControls->Save_t(GAME_COMMON_CONTROLS_PATH);
      gm_pControls->Load_t(GAME_COMMON_CONTROLS_PATH);

    } catch (char *strError) {
      CPrintF(TRANS("Cannot reload common controls:\n%s\n"), strError);
    }
  }

  CGameMenu::EndMenu();
}