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
#include "MenuManager.h"
#include "MSinglePlayerNew.h"

// [Cecil] Convert all characters to uppercase
static inline void ToUpper(CTString &str) {
  const INDEX ct = str.Length();

  // Only capitalize decorated characters, ignoring characters in color tags
  for (INDEX i = 0; i < ct; i++) {
    char &ch = str.str_String[IData::GetDecoratedChar(str, i)];
    ch = toupper(ch);
  }
};

// [Cecil] Open gameplay customization config
static void OpenGameplayCustomization(void) {
  _pGUIM->gmVarMenu.gm_mgTitle.SetName(TRANS("GAME CUSTOMIZATION"));
  _pGUIM->gmVarMenu.gm_fnmMenuCFG = CTString("Scripts\\ClassicsPatch\\03_GameplaySettings.cfg");
  _pGUIM->gmVarMenu.SetParentMenu(&_pGUIM->gmSinglePlayerNewMenu);

  ChangeToMenu(&_pGUIM->gmVarMenu);
};

void CSinglePlayerNewMenu::Initialize_t(void) {
  // intialize single player new menu
  gm_mgTitle.SetName(LOCALIZE("NEW GAME"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  // [Cecil] Add up to 16 mod difficulties
  INDEX ct = ClampUp(ClassicsModData_CountNamedDiffs(), (int)ARRAYCOUNT(astrDifficultyRadioTexts));

  for (INDEX iAdd = 0; iAdd < ct; iAdd++)
  {
    const ModDifficulty_t &diff = *ClassicsModData_GetDiff(iAdd);

    // Make name all uppercase and translate both strings
    CTString strName = diff.m_strName;
    CTString strTip = diff.m_strTip;
    ToUpper(strName);

    // Add new difficulty
    CMGButton &mg = gm_amgDifficulties.Push();
    mg.mg_iIndex = iAdd;
    mg.mg_pActivatedFunction = NULL;

    mg.SetText(TRANSV(strName));
    mg.mg_strTip = TRANSV(strTip);
    mg.mg_bfsFontSize = (ct > 9 ? BFS_MEDIUM : BFS_LARGE);
    mg.mg_bMental = diff.m_bFlash; // Text blinking

    AddChild(&mg);
  }

  // Add dummy difficulty button
  ct = gm_amgDifficulties.Count();

  if (ct == 0) {
    CMGButton &mg = gm_amgDifficulties.Push();
    mg.SetText(TRANS("INVALID"));
    mg.mg_strTip = TRANS("this mod has no difficulties set up");
    mg.mg_bfsFontSize = BFS_LARGE;

    mg.mg_iIndex = 0;
    mg.mg_pActivatedFunction = NULL;
    mg.mg_bEnabled = FALSE;
    AddChild(&mg);

    ct = 1;
  }

  // Link buttons together
  BOOL bMediumFont = (ct > 9);

  for (INDEX iLink = 0; iLink < ct; iLink++) {
    CMGButton &mg = gm_amgDifficulties[iLink];

    // Adjust position
    if (bMediumFont) {
      mg.mg_boxOnScreen = BoxMediumRow(iLink - 1);
    } else {
      mg.mg_boxOnScreen = BoxBigRow(iLink);
    }

    mg.mg_pmgUp = &gm_amgDifficulties[(iLink + ct - 1) % ct];
    mg.mg_pmgDown = &gm_amgDifficulties[(iLink + 1) % ct];
  }

  // [Cecil] Game customization button
  gm_mgCustomize.SetText(TRANS("Customize game"));
  gm_mgCustomize.mg_strTip = TRANS("^cff5f5fWARNING: Gameplay extensions are incompatible with vanilla game!");
  gm_mgCustomize.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgCustomize.mg_boxOnScreen = BoxLeftColumn(15.0f);
  gm_mgCustomize.mg_iCenterI = -1;

  gm_mgCustomize.mg_pmgRight = &gm_amgDifficulties[0];
  gm_mgCustomize.mg_pActivatedFunction = &OpenGameplayCustomization;

  AddChild(&gm_mgCustomize);
};

void CSinglePlayerNewMenu::StartMenu(void) {
  CGameMenu::StartMenu();

  // [Cecil] Toggle difficulties based on activity
  const INDEX ct = gm_amgDifficulties.Count();

  for (INDEX i = 0; i < ct; i++) {
    CMGButton &mg = gm_amgDifficulties[i];

    if (ClassicsModData_IsDiffActive(i)) {
      mg.Appear();
      gm_amgDifficulties[(i + ct - 1) % ct].mg_pmgDown = &mg;
      gm_amgDifficulties[(i + 1) % ct].mg_pmgUp = &mg;

    } else {
      mg.Disappear();
      gm_amgDifficulties[4].mg_pmgDown = &gm_amgDifficulties[6 % ct];
      gm_amgDifficulties[6 % ct].mg_pmgUp = &gm_amgDifficulties[4];
    }
  }
};
