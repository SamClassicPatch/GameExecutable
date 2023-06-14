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

// This file contains additional functions called from starters.

#include "StdH.h"

#include "MenuManager.h"
#include "MenuStartersAF.h"
#include "MenuStarters.h"
#include "MenuStuff.h"

static CTFileName _fnDemoToPlay;
static CTFileName _fnGameToLoad;

extern CTString sam_strNetworkSettings;

extern CTFileName _fnmModSelected;
extern CTString _strModURLSelected;
extern CTString _strModServerSelected;

BOOL LSLoadSinglePlayer(const CTFileName &fnm) {
  GetGameAPI()->SetStartSplitCfg(CGame::SSC_PLAY1);

  // [Cecil] Reset start player indices
  GetGameAPI()->ResetStartPlayers();
  GetGameAPI()->SetStartPlayer(0, GetGameAPI()->GetPlayerForSP());

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);

  if (_pGame->LoadGame(fnm)) {
    StopMenus();
    _gmRunningGameMode = GM_SINGLE_PLAYER;

  } else {
    _gmRunningGameMode = GM_NONE;
  }
  return TRUE;
}

BOOL LSLoadNetwork(const CTFileName &fnm) {
  // call local players menu
  _fnGameToLoad = fnm;
  StartSelectPlayersMenuFromNetworkLoad();
  return TRUE;
}

BOOL LSLoadSplitScreen(const CTFileName &fnm) {
  // call local players menu
  _fnGameToLoad = fnm;
  StartSelectPlayersMenuFromSplitScreenLoad();
  return TRUE;
}

void StartDemoPlay(void) {
  GetGameAPI()->SetStartSplitCfg(CGame::SSC_OBSERVER);
  // play the demo
  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);
  if (_pGame->StartDemoPlay(_fnDemoToPlay)) {
    // exit menu and pull up the console
    StopMenus();
    if (GetGameAPI()->GetConState() != CS_OFF) {
      GetGameAPI()->SetConState(CS_TURNINGOFF);
    }
    _gmRunningGameMode = GM_DEMO;
  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

extern BOOL LSLoadDemo(const CTFileName &fnm) {
  // call local players menu
  _fnDemoToPlay = fnm;
  StartDemoPlay();
  return TRUE;
}

BOOL LSLoadPlayerModel(const CTFileName &fnm) {
  // get base filename
  CTString strBaseName = fnm.FileName();
  // set it for current player
  CPlayerCharacter &pc = *GetGameAPI()->GetPlayerCharacter(*_pGUIM->gmPlayerProfile.gm_piCurrentPlayer);
  CPlayerSettings *pps = (CPlayerSettings *)pc.pc_aubAppearance;
  memset(pps->ps_achModelFile, 0, sizeof(pps->ps_achModelFile));
  strncpy(pps->ps_achModelFile, strBaseName, sizeof(pps->ps_achModelFile));

  MenuGoToParent();
  return TRUE;
}

BOOL LSLoadControls(const CTFileName &fnm) {
  try {
    ControlsMenuOn();
    GetGameAPI()->GetControls()->Load_t(fnm);
    ControlsMenuOff();
  } catch (char *strError) {
    CPrintF("%s", strError);
  }

  MenuGoToParent();
  return TRUE;
}

BOOL LSLoadAddon(const CTFileName &fnm) {
  extern INDEX _iAddonExecState;
  extern CTFileName _fnmAddonToExec;
  _iAddonExecState = 1;
  _fnmAddonToExec = fnm;
  return TRUE;
}

BOOL LSLoadMod(const CTFileName &fnm) {
  _fnmModSelected = fnm;
  extern void ModConfirm(void);
  ModConfirm();
  return TRUE;
}

BOOL LSLoadCustom(const CTFileName &fnm) {
  _pGUIM->gmVarMenu.gm_mgTitle.SetName(LOCALIZE("ADVANCED OPTIONS"));
  //  LoadStringVar(fnm.NoExt()+".des", mgVarTitle.mg_strText);
  //  mgVarTitle.mg_strText.OnlyFirstLine();
  _pGUIM->gmVarMenu.gm_fnmMenuCFG = fnm;
  _pGUIM->gmVarMenu.SetParentMenu(&_pGUIM->gmLoadSaveMenu);
  ChangeToMenu(&_pGUIM->gmVarMenu);
  return TRUE;
}

// [Cecil] Load config settings from the patch
BOOL LSLoadPatchConfig(const CTFileName &fnm) {
  _pGUIM->gmVarMenu.gm_mgTitle.SetName(TRANS("CLASSICS PATCH"));
  _pGUIM->gmVarMenu.gm_fnmMenuCFG = fnm;
  _pGUIM->gmVarMenu.SetParentMenu(&_pGUIM->gmLoadSaveMenu);

  ChangeToMenu(&_pGUIM->gmVarMenu);
  return TRUE;
};

BOOL LSLoadNetSettings(const CTFileName &fnm) {
  sam_strNetworkSettings = fnm;
  CTString strCmd;
  strCmd.PrintF("include \"%s\"", (const char *)sam_strNetworkSettings);
  _pShell->Execute(strCmd);

  MenuGoToParent();
  return TRUE;
}

// same function for saving in singleplay, network and splitscreen
BOOL LSSaveAnyGame(const CTFileName &fnm) {
  if (_pGame->SaveGame(fnm)) {
    StopMenus();

    // [Cecil] Save game for Core
    GetAPI()->OnGameSave(fnm);

    return TRUE;
  }

  return FALSE;
}

BOOL LSSaveDemo(const CTFileName &fnm) {
  // save the demo
  if (_pGame->StartDemoRec(fnm)) {
    StopMenus();

    return TRUE;
  }

  return FALSE;
}

void StartNetworkLoadGame(void) {
  GetGameAPI()->SetStartSplitCfg(GetGameAPI()->GetMenuSplitCfg());

  // [Cecil] Set start players from menu players
  GetGameAPI()->SetStartPlayersFromMenuPlayers();

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_SERVER);

  if (_pGame->LoadGame(_fnGameToLoad)) {
    StopMenus();
    _gmRunningGameMode = GM_NETWORK;

  } else {
    _gmRunningGameMode = GM_NONE;
  }
}

void StartSplitScreenGameLoad(void) {
  GetGameAPI()->SetStartSplitCfg(GetGameAPI()->GetMenuSplitCfg());

  // [Cecil] Set start players from menu players
  GetGameAPI()->SetStartPlayersFromMenuPlayers();

  GetGameAPI()->SetNetworkProvider(CGameAPI::NP_LOCAL);

  if (_pGame->LoadGame(_fnGameToLoad)) {
    StopMenus();
    _gmRunningGameMode = GM_SPLIT_SCREEN;

  } else {
    _gmRunningGameMode = GM_NONE;
  }
}