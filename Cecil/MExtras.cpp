/* Copyright (c) 2023-2024 Dreamy Cecil
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

#include "Credits.h"
#include "MExtras.h"
#include "GUI/Menus/MenuPrinting.h"
#include "GUI/Menus/MenuStarters.h"
#include "GUI/Menus/MenuManager.h"

#define URL_WIKI CLASSICSPATCH_URL_PROJECT "/wiki"

// Display game credits
static void StartGameCreditsMenu(void) {
  // Renders credits until the end or any input
  extern void QuitScreenLoop(void);
  QuitScreenLoop();

  Credits_Off();
};

// Open documentation
static void OpenWiki(void) {
  extern void RunBrowser(const char *);
  RunBrowser(URL_WIKI);
};

static void OpenWikiConfirm(void) {
  CConfirmMenu &gm = _pGUIM->gmConfirmMenu;

  gm._pConfimedYes = &OpenWiki;
  gm._pConfimedNo = NULL;

  CTString strPrompt = TRANS("The following link will open in your web browser:\n");
  gm.SetText(strPrompt + URL_WIKI, TRANS("PROCEED"), TRANS("CANCEL"));
  gm.SetParentMenu(&_pGUIM->gmExtras);
  gm.BeSmall();
  ChangeToMenu(&gm);
};

static void RestartGameConfirm(void) {
  CConfirmMenu &gm = _pGUIM->gmConfirmMenu;

  gm._pConfimedYes = &RestartGame;
  gm._pConfimedNo = NULL;

  gm.SetText(LOCALIZE("ARE YOU SERIOUS?"));
  gm.SetParentMenu(&_pGUIM->gmExtras);
  gm.BeLarge();
  ChangeToMenu(&gm);
};

// Initialize extras
void CExtrasMenu::Initialize_t(void) {
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(TRANS("EXTRAS"));
  AddChild(&gm_mgTitle);

  gm_mgMods.SetText(LOCALIZE("MODS"));
  gm_mgMods.mg_strTip = LOCALIZE("run one of installed game modifications");
  gm_mgMods.mg_bfsFontSize = BFS_LARGE;
  gm_mgMods.mg_boxOnScreen = BoxBigRow(1.0f);

  gm_mgMods.mg_pmgUp = &gm_mgDocs;
  gm_mgMods.mg_pmgDown = &gm_mgPatchCredits;
  gm_mgMods.mg_pActivatedFunction = &StartModsLoadMenu;
  AddChild(&gm_mgMods);

  gm_mgPatchCredits.SetText(TRANS("PATCH CREDITS"));
  gm_mgPatchCredits.mg_strTip = TRANS("credits for Classics Patch");
  gm_mgPatchCredits.mg_bfsFontSize = BFS_LARGE;
  gm_mgPatchCredits.mg_boxOnScreen = BoxBigRow(2.0f);

  gm_mgPatchCredits.mg_pmgUp = &gm_mgMods;
  gm_mgPatchCredits.mg_pmgDown = &gm_mgGameCredits;
  gm_mgPatchCredits.mg_pActivatedFunction = &StartPatchCreditsMenu;
  AddChild(&gm_mgPatchCredits);

  gm_mgGameCredits.SetText(TRANS("GAME CREDITS"));
  gm_mgGameCredits.mg_strTip = TRANS("credits for the game or a mod");
  gm_mgGameCredits.mg_bfsFontSize = BFS_LARGE;
  gm_mgGameCredits.mg_boxOnScreen = BoxBigRow(3.0f);

  gm_mgGameCredits.mg_pmgUp = &gm_mgPatchCredits;
  gm_mgGameCredits.mg_pmgDown = &gm_mgRestart;
  gm_mgGameCredits.mg_pActivatedFunction = &StartGameCreditsMenu;
  AddChild(&gm_mgGameCredits);

  gm_mgDocs.SetText(TRANS("DOCUMENTATION"));
  gm_mgDocs.mg_strTip = LOCALIZE("visit classics patch wiki for documentation");
  gm_mgDocs.mg_bfsFontSize = BFS_LARGE;
  gm_mgDocs.mg_boxOnScreen = BoxBigRow(4.0f);

  gm_mgDocs.mg_pmgUp = &gm_mgGameCredits;
  gm_mgDocs.mg_pmgDown = &gm_mgRestart;
  gm_mgDocs.mg_pActivatedFunction = &OpenWikiConfirm;
  AddChild(&gm_mgDocs);

  gm_mgRestart.SetText(TRANS("RESTART"));
  gm_mgRestart.mg_strTip = TRANS("reload current game or mod immediately");
  gm_mgRestart.mg_bfsFontSize = BFS_LARGE;
  gm_mgRestart.mg_boxOnScreen = BoxBigRow(5.0f);

  gm_mgRestart.mg_pmgUp = &gm_mgDocs;
  gm_mgRestart.mg_pmgDown = &gm_mgMods;
  gm_mgRestart.mg_pActivatedFunction = &RestartGameConfirm;
  AddChild(&gm_mgRestart);
};
