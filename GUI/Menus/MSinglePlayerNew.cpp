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
#include "MSinglePlayerNew.h"

// [Cecil] Convert all characters to uppercase
static inline void ToUpper(CTString &str) {
  char *pch = str.str_String;

  while (*pch != '\0') {
    *pch = toupper(*pch);
    pch++;
  }
};

void CSinglePlayerNewMenu::Initialize_t(void) {
  // intialize single player new menu
  gm_mgTitle.SetName(LOCALIZE("NEW GAME"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  // [Cecil] Add up to 16 mod difficulties
  INDEX ct = ClampUp(CoreVarData().CountDiffs(), (INDEX)16);

  for (INDEX iAdd = 0; iAdd < ct; iAdd++) {
    // Get difficulty name from the API
    const CCoreVariables::Difficulty &diff = CoreVarData().GetDiff(iAdd);

    // No more difficulties
    if (diff.strName == "") break;

    // Make all uppercase and translate
    CTString strName = diff.strName;
    ToUpper(strName);

    // Add new difficulty
    CMGButton &mg = gm_amgDifficulties.Push();
    mg.mg_iIndex = iAdd;
    mg.mg_pActivatedFunction = NULL;

    mg.SetText(TRANSV(strName));
    mg.mg_strTip = TRANSV(diff.strTip);
    mg.mg_bfsFontSize = (ct > 9 ? BFS_MEDIUM : BFS_LARGE);
    mg.mg_bMental = diff.bFlash; // Text blinking

    AddChild(&mg);
  }

  // Link buttons together
  ct = gm_amgDifficulties.Count();
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
};

void CSinglePlayerNewMenu::StartMenu(void) {
  CGameMenu::StartMenu();

  // [Cecil] Toggle difficulties based on activity
  const INDEX ct = gm_amgDifficulties.Count();

  for (INDEX i = 0; i < ct; i++) {
    CMGButton &mg = gm_amgDifficulties[i];

    if (CoreVarData().GetDiff(i).IsActive()) {
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
