/* Copyright (c) 2023 Dreamy Cecil
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

#include "MExtras.h"
#include "GUI/Menus/MenuPrinting.h"
#include "GUI/Menus/MenuStarters.h"
#include "GUI/Menus/MenuManager.h"

// Restart the game client
static void RestartGame(void) {
  _bRunning = FALSE;
  _bRestartGameClient = TRUE;
};

// Initialize extras
void CExtrasMenu::Initialize_t(void) {
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(TRANS("EXTRAS"));
  AddChild(&gm_mgTitle);

  gm_mgMods.SetText(LOCALIZE("MODS"));
  gm_mgMods.mg_strTip = LOCALIZE("run one of installed game modifications");
  gm_mgMods.mg_bfsFontSize = BFS_LARGE;
  gm_mgMods.mg_boxOnScreen = BoxBigRow(2.0f);

  gm_mgMods.mg_pmgUp = &gm_mgRestart;
  gm_mgMods.mg_pmgDown = &gm_mgCredits;
  gm_mgMods.mg_pActivatedFunction = &StartModsLoadMenu;
  AddChild(&gm_mgMods);

  gm_mgCredits.SetText(TRANS("CREDITS"));
  gm_mgCredits.mg_strTip = TRANS("credits for Classics Patch");
  gm_mgCredits.mg_bfsFontSize = BFS_LARGE;
  gm_mgCredits.mg_boxOnScreen = BoxBigRow(3.0f);

  gm_mgCredits.mg_pmgUp = &gm_mgMods;
  gm_mgCredits.mg_pmgDown = &gm_mgRestart;
  gm_mgCredits.mg_pActivatedFunction = &StartCreditsMenu;
  AddChild(&gm_mgCredits);

  gm_mgRestart.SetText(TRANS("RESTART"));
  gm_mgRestart.mg_strTip = TRANS("reload current game or mod immediately");
  gm_mgRestart.mg_bfsFontSize = BFS_LARGE;
  gm_mgRestart.mg_boxOnScreen = BoxBigRow(4.0f);

  gm_mgRestart.mg_pmgUp = &gm_mgCredits;
  gm_mgRestart.mg_pmgDown = &gm_mgMods;
  gm_mgRestart.mg_pActivatedFunction = &RestartGame;
  AddChild(&gm_mgRestart);
};
